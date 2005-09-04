
/*
 * DUMA - Red-Zone memory allocator.
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
 * License: GNU GPL (GNU General Public License, see COPYING-GPL)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * FILE CONTENTS:
 * --------------
 * This is a special version of malloc() and company for debugging software
 * that is suspected of overrunning or underrunning the boundaries of a
 * malloc buffer, or touching free memory.
 *
 * It arranges for each malloc buffer to be followed (or preceded)
 * in the address space by an inaccessable virtual memory page,
 * and for free memory to be inaccessable. If software touches the
 * inaccessable page, it will get an immediate segmentation
 * fault. It is then trivial to uncover the offending code using a debugger.
 *
 * An advantage of this product over most malloc debuggers is that this one
 * detects reading out of bounds as well as writing, and this one stops on
 * the exact instruction that causes the error, rather than waiting until the
 * next boundary check.
 *
 * There is one product that debugs malloc buffer overruns
 * better than DUMA: "Purify" from Purify Systems, and that's only
 * a small part of what Purify does. I'm not affiliated with Purify, I just
 * respect a job well done.
 *
 * This version of malloc() should not be linked into production software,
 * since it tremendously increases the time and memory overhead of malloc().
 * Each malloc buffer will consume a minimum of two virtual memory pages,
 * this is 16 kilobytes on many systems. On some systems it will be necessary
 * to increase the amount of swap space in order to debug large programs that
 * perform lots of allocation, because of the per-buffer overhead.
 */

#ifndef DUMA_NO_DUMA
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>

#ifndef WIN32
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/mman.h>
#else
  #define WIN32_LEAN_AND_MEAN 1
  #include <windows.h>
  #include <winbase.h>
  #include <io.h>

  typedef LPVOID caddr_t;
  typedef unsigned u_int;
#endif

#ifdef _MSC_VER
  #include <crtdbg.h>
#endif

#include "duma.h"
#include "dumaint.h"
#include "print.h"
#include "sem_inc.h"
#include "paging.h"

static const char  version[] = "\n"
"DUMA 2.4.18\n"
"Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH\n";
"Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>\n"


static const char unknown_file[] =
  "UNKNOWN (use #include \"duma.h\")";


#ifndef DUMA_NO_LEAKDETECTION
#define DUMA_PARAMLIST_FL       , const char * filename, int lineno
#define DUMA_PARAMS_FL          , filename, lineno
#define DUMA_PARAMS_UK          , unknown_file, 0
#else
#define DUMA_PARAMLIST_FL
#define DUMA_PARAMS_FL
#define DUMA_PARAMS_UK
#endif


/*
 * MEMORY_CREATION_SIZE is the amount of memory to get from the operating
 * system at one time. We'll break that memory down into smaller pieces for
 * malloc buffers. One megabyte is probably a good value.
 */
#define      MEMORY_CREATION_SIZE  1024 * 1024

enum _DUMA_SlotState
{
    DUMAST_EMPTY            /* slot not in use */
  , DUMAST_FREE             /* internal memory reserved, unused by user */
  , DUMAST_IN_USE           /* memory in use by allocator; see following enum AllocType */
  , DUMAST_ALL_PROTECTED    /* memory no more used by allocator; memory is not deallocated but protected */
  , DUMAST_BEGIN_PROTECTED  /* most memory deallocated, but not page covering userAddress:
                           * slot holds userAddress, userSize and allocator.
                           */
};

enum _DUMA_Slot_FileSource
{
    DUMAFS_EMPTY            /* no filename, lineno */
  , DUMAFS_ALLOCATION       /* filename, lineno from allocation */
  , DUMAFS_DEALLOCATION     /* filename, lineno from deallocation */
};

/*
 * Struct Slot contains all of the information about a malloc buffer except
 * for the contents of its memory.
 */
struct _DUMA_Slot
{
  void            * internalAddress;
  void            * userAddress;
  void            * protAddress;
  size_t            internalSize;
  size_t            userSize;

#if 0
  /* just for checking compiler warnings / errors */
  enum _DUMA_SlotState        state;
  enum _DUMA_Allocator        allocator;
  #ifndef DUMA_NO_LEAKDETECTION
  enum _DUMA_Slot_FileSource  fileSource;
  #endif
#else
  /* save (some) space in production */
  unsigned short    state       :16;
  #ifdef DUMA_NO_LEAKDETECTION
  unsigned short    allocator   :16;
  #else
  unsigned short    allocator   :8;
  unsigned short    fileSource  :8;
  #endif
#endif

#ifndef DUMA_NO_LEAKDETECTION
#ifdef DUMA_USE_FRAMENO
  int               frame;
#endif
  char            * filename;   /* filename of allocation */
  int               lineno;     /* linenumber of allocation */
#endif

#ifdef DUMA_EXPLICIT_INIT
  int               slackfill;
#endif
};

enum _DUMA_AllocType
{
    DUMAAT_INTERNAL
  , DUMAAT_MALLOC
  , DUMAAT_NEW_ELEM
  , DUMAAT_NEW_ARRAY
};

static struct _DUMA_AllocDesc
{
  char                * name;
  enum _DUMA_AllocType    type;
}
_duma_allocDesc[] =
{
    { "duma allocate()"   , DUMAAT_INTERNAL  }
  , { "duma deallocate()" , DUMAAT_INTERNAL  }
  , { "malloc()"            , DUMAAT_MALLOC    }
  , { "calloc()"            , DUMAAT_MALLOC    }
  , { "free()"              , DUMAAT_MALLOC    }
  , { "memalign()"          , DUMAAT_MALLOC    }
  , { "realloc()"           , DUMAAT_MALLOC    }
  , { "valloc()"            , DUMAAT_MALLOC    }
  , { "strdup()"            , DUMAAT_MALLOC    }
  , { "new (element)"       , DUMAAT_NEW_ELEM  }
  , { "delete (element)"    , DUMAAT_NEW_ELEM  }
  , { "new[] (array)"       , DUMAAT_NEW_ARRAY }
  , { "[]delete (array)"    , DUMAAT_NEW_ARRAY }
};

#ifndef DUMA_NO_LEAKDETECTION
#ifdef DUMA_USE_FRAMENO
static int    frameno = 0;
#endif
#endif

/*
 * DUMA_DISABLE_BANNER is a global variable used to control whether
 * DUMA prints its usual startup message. If the value is
 * -1, it will be set from the environment default to 0 at run time.
 */
static int    DUMA_DISABLE_BANNER = -1;

/*
 * DUMA_ALIGNMENT is a global variable used to control the default alignment
 * of buffers returned by malloc(), calloc(), and realloc(). It is all-caps
 * so that its name matches the name of the environment variable that is used
 * to set it. This gives the programmer one less name to remember.
 */
int           DUMA_ALIGNMENT = sizeof(int);

/*
 * DUMA_PROTECT_BELOW is used to modify the behavior of the allocator. When
 * its value is non-zero, the allocator will place an inaccessable page
 * immediately _before_ the malloc buffer in the address space, instead
 * of _after_ it. Use this to detect malloc buffer under-runs, rather than
 * over-runs. It won't detect both at the same time, so you should test your
 * software twice, once with this value clear, and once with it set.
 */
int           DUMA_PROTECT_BELOW = 0;

/*
 * DUMA_FILL is set to 0-255 if DUMA should fill all new allocated
 * memory with the specified value. Set to -1 when DUMA should not
 * initialise allocated memory.
 * default is set to initialise with 255, cause many programs rely on
 * initialisation to 0!
 */
int           DUMA_FILL = 255;

/*
 * DUMA_SLACKFILL is set to 0-255. The slack / no mans land of all new allocated
 * memory is filled with the specified value.
 * default is set to initialise with 0xAA (=binary 10101010)
 * initialisation to 0!
 */
static int    DUMA_SLACKFILL = 0xAA;

/*
 * DUMA_PROTECT_FREE is used to control the disposition of memory that is
 * released using free(). It is all-caps so that its name
 * matches the name of the environment variable that is used to set it.
 * If its value is non-zero, memory released by free is made inaccessable.
 * Any software that touches free memory will then get a segmentation fault.
 * Depending on your application and your resources you may tell
 * DUMA not to use this memory ever again by setting a negative
 * value f.e. -1.
 * You can tell DUMA to limit the sum of protected memory by setting
 * a positive value, which is interpreted in kB.
 * If its value is zero, freed memory will be available for reallocation,
 * but will still be inaccessable until it is reallocated.
 */
static long   DUMA_PROTECT_FREE = -1L;

/*
 * DUMA_MAX_ALLOC is used to control the maximum memory print of the program
 * in total: When the sum of allocated and protected memory would exceed
 * this value in kB, the protected memory is freed/deleted.
 */
static long   DUMA_MAX_ALLOC = -1L;

/*
 * DUMA_ALLOW_MALLOC_0 is set if DUMA is to allow malloc(0). I
 * trap malloc(0) by default because it is a common source of bugs.
 * But you should know the allocation with size 0 is ANSI conform.
 */
static int    DUMA_ALLOW_MALLOC_0 = 1;

/*
 * DUMA_MALLOC_FAILEXIT controls the behaviour of DUMA when
 * malloc() fails and would return NULL. But most applications don't
 * check the return value for errors ... so
 * default to Exit on Fail
 */
static int    DUMA_MALLOC_FAILEXIT = 1;

/*
 * DUMA_FREE_ACCESS is set if DUMA is to write access memory before
 * freeing it. This makes easier using watch expressions in debuggers as the
 * process is interrupted even if the memory is going to be freed.
 */
static int    DUMA_FREE_ACCESS = 0;

/*
 * DUMA_FREE_WIPES is set if DUMA is to wipe the memory content
 * of freed blocks. This makes it easier to check if memory is freed or
 * not
 */
static int    DUMA_FREE_WIPES = 0;

/*
 * DUMA_SHOW_ALLOC is set if DUMA is to print all allocations
 * and deallocations to the console. Although this generates a lot
 * of messages, the option can be useful to detect inefficient code
 * containing many allocations / deallocations
 */
static int    DUMA_SHOW_ALLOC = 0;

/*
 * _DUMA_allocList points to the array of slot structures used to manage the
 * malloc arena.
 */
struct _DUMA_Slot * _duma_allocList = 0;

/*
 * _duma_allocListSize is the size of the allocation list. This will always
 * be a multiple of the page size.
 */
static size_t _duma_allocListSize = 0;

/*
 * slotCount is the number of Slot structures in allocationList.
 */
static size_t slotCount = 0;

/*
 * unUsedSlots is the number of Slot structures that are currently available
 * to represent new malloc buffers. When this number gets too low, we will
 * create new slots.
 */
static size_t unUsedSlots = 0;

/*
 * slotsPerPage is the number of slot structures that fit in a virtual
 * memory page.
 */
static size_t slotsPerPage = 0;

/*
 * internal variable: sum of allocated -freed +protected memory in kB
 */
static long   sumAllocatedMem = 0;

/*
 * internal variable: sum of allocated memory in kB
 */
static long   sumTotalAllocatedMem = 0;

/*
 * internal variable: sum of protected memory in kB
 */
static long   sumProtectedMem = 0;

/*
 * internal variable: number of deallocations processed so far
 */
static long numDeallocs = 0;

/*
 * internal variable: number of allocations processed so far
 */
static long numAllocs = 0;

/*
 * internal variable: is DUMA_init() already done
 */
static int duma_init_done = 0;


/*
 * include helper functions
 */
#include "duma_hlp.h"



void _duma_assert(const char * exprstr, const char * filename, int lineno)
{
  int *pcAddr = 0;
  DUMA_Print("\nDUMA: DUMA_ASSERT(%s) failed at\n%s(%d)\n", exprstr, filename, lineno );
  /* this is "really" bad, but it works. assert() from assert.h system header
   * stops only the current thread but the program goes on running under MS Visual C++.
   * This way the program definitely halts.
   */
  while (1)
    *pcAddr++ = 0;
}


#ifndef DUMA_EXPLICIT_INIT
static
#endif
void duma_init(void)
{
  char            * string;
  void            * testAlloc;

  if ( (string = getenv("DUMA_DISABLE_BANNER")) != 0 )
    DUMA_DISABLE_BANNER = (atoi(string) != 0);
  if ( !DUMA_DISABLE_BANNER )
    DUMA_Print(version);

  /*
   * Import the user's environment specification of the default
   * alignment for malloc(). We want that alignment to be under
   * user control, since smaller alignment lets us catch more bugs,
   * however some software will break if malloc() returns a buffer
   * that is not word-aligned.
   *
   * I would like
   * alignment to be zero so that we could catch all one-byte
   * overruns, however if malloc() is asked to allocate an odd-size
   * buffer and returns an address that is not word-aligned, or whose
   * size is not a multiple of the word size, software breaks.
   * This was the case with the Sun string-handling routines,
   * which can do word fetches up to three bytes beyond the end of a
   * string. I handle this problem in part by providing
   * byte-reference-only versions of the string library functions, but
   * there are other functions that break, too. Some in X Windows, one
   * in Sam Leffler's TIFF library, and doubtless many others.
   */
  if ( (string = getenv("DUMA_ALIGNMENT")) != 0 )
    DUMA_ALIGNMENT = (size_t)atoi(string);

  /*
   * See if the user wants to protect the address space below a buffer,
   * rather than that above a buffer.
   */
  if ( (string = getenv("DUMA_PROTECT_BELOW")) != 0 )
    DUMA_PROTECT_BELOW = (atoi(string) != 0);

  /*
   * See if the user wants to protect memory that has been freed until
   * the program exits, rather than until it is re-allocated.
   * =-1 protect as much free'd memory as possible
   * =0 do not protect free'd memory
   * =N protect memory up to N kB
   */
  if ( (string = getenv("DUMA_PROTECT_FREE")) != 0 )
    DUMA_PROTECT_FREE = atol(string);

  /*
   * See if the user has a memory usage limit. This controls the maximum
   * memory print of the program in total: The sum of allocated and protected
   * memory won't exceed this limit.
   * =-1 use as much memory as possible
   * =N limit total memory usage to N kB
   */
  if ( (string = getenv("DUMA_MAX_ALLOC")) != 0 )
    DUMA_MAX_ALLOC = atol(string);

  /*
   * See if the user wants to allow malloc(0).
   */
  if ( (string = getenv("DUMA_ALLOW_MALLOC_0")) != 0 )
    DUMA_ALLOW_MALLOC_0 = (atoi(string) != 0);

  /*
   * See if the user wants to exit on malloc() failure
   */
  if ( (string = getenv("DUMA_MALLOC_FAILEXIT")) != 0 )
    DUMA_MALLOC_FAILEXIT = (atoi(string) != 0);

  /*
   * See if the user wants to write access freed memory
   */
  if ( (string = getenv("DUMA_FREE_ACCESS")) != 0 )
    DUMA_FREE_ACCESS = (atoi(string) != 0);

  /*
   * See if the user wants us to wipe out freed memory
   */
  if ( (string = getenv("DUMA_FREE_WIPES")) != 0 )
    DUMA_FREE_WIPES = (atoi(string) != 0);

  /*
   * Check if we should be filling new memory with a value.
   */
  if ( (string = getenv("DUMA_FILL")) != 0)
  {
    DUMA_FILL = atoi(string);
    if ( -1 != DUMA_FILL )
      DUMA_FILL &= 255;
  }

  /*
   * Check with which value the memories no mans land is filled
   */
  if ( (string = getenv("DUMA_SLACKFILL")) != 0)
    DUMA_SLACKFILL = atoi(string);
  DUMA_SLACKFILL &= 255;

  /*
   * See if the user wants to see allocations / frees
   */
  if ( (string = getenv("DUMA_SHOW_ALLOC")) != 0 )
    DUMA_SHOW_ALLOC = (atoi(string) != 0);


  /*
   * Register atexit()
   */
#ifndef DUMA_NO_LEAKDETECTION
  DUMA_Print("\nDUMA: Registering with atexit().\n"
#ifdef WIN32
             "DUMA: If this hangs, change the library load order with DUMA_EXPLICIT_INIT.\n");
#else
             "DUMA: If this hangs, change the library load order with DUMA_EXPLICIT_INIT or LD_PRELOAD.\n");
#endif
  if ( atexit( DUMA_delFrame ) )
    DUMA_Abort("Cannot register exit function.\n");
  DUMA_Print("DUMA: Registration was successful.\n");
#endif

  /* initialize semaphoring */
  DUMA_INIT_SEMAPHORE();

#ifndef DUMA_NO_GLOBAL_MALLOC_FREE
  /*
   * Check whether malloc and free is available
   */
  testAlloc = malloc(123);
  if (numAllocs == 0)
    DUMA_Abort("malloc() is not bound to duma.\nDUMA Aborting: Preload lib with 'LD_PRELOAD=libduma.so <prog>'.\n");

  free(testAlloc);
  if (numDeallocs == 0)
    DUMA_Abort("free() is not bound to duma.\nDUMA Aborting: Preload lib with 'LD_PRELOAD=libduma.so <prog>'.\n");
#endif

  duma_init_done = 1;
}


/*
 * _duma_init sets up the memory allocation arena and the run-time
 * configuration information.
 */
void _duma_init(void)
{
  size_t            size = MEMORY_CREATION_SIZE;
  struct _DUMA_Slot * slot;

#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
  if (duma_init_done)
#endif
  DUMA_GET_SEMAPHORE();
#endif

  /*
   * Figure out how many Slot structures to allocate at one time.
   */
  slotCount = slotsPerPage = DUMA_PAGE_SIZE / sizeof(struct _DUMA_Slot);
  _duma_allocListSize = DUMA_PAGE_SIZE;

  if ( size < _duma_allocListSize )
    size = _duma_allocListSize;

  size = ( size + DUMA_PAGE_SIZE -1 ) & ~( DUMA_PAGE_SIZE -1 );

  /*
   * Allocate memory, and break it up into two malloc buffers. The
   * first buffer will be used for Slot structures, the second will
   * be marked free.
   */
  slot = _duma_allocList = (struct _DUMA_Slot *)Page_Create(size, 1/*=exitonfail*/);
  memset((char *)_duma_allocList, 0, _duma_allocListSize);

  /* enter _duma_allocList as slot to allow call to free() when doing allocateMoreSlots() */
  slot[0].internalAddress   = slot[0].userAddress = _duma_allocList;
  slot[0].internalSize      = slot[0].userSize    = _duma_allocListSize;
  slot[0].state             = DUMAST_IN_USE;
  slot[0].allocator         = EFA_INT_ALLOC;
#ifndef DUMA_NO_LEAKDETECTION
  slot[0].fileSource        = DUMAFS_ALLOCATION;
#ifdef DUMA_USE_FRAMENO
  slot[0].frame             = 0;
#endif
  slot[0].filename          = __FILE__;
  slot[0].lineno            = __LINE__;
#endif
  if ( size > _duma_allocListSize )
  {
    slot[1].internalAddress = slot[1].userAddress
                            = ((char *)slot[0].internalAddress) + slot[0].internalSize;
    slot[1].internalSize    = slot[1].userSize
                            =   size - slot[0].internalSize;
    slot[1].state           = DUMAST_FREE;
    slot[1].allocator       = EFA_INT_ALLOC;
#ifndef DUMA_NO_LEAKDETECTION
    slot[1].fileSource      = DUMAFS_ALLOCATION;
#ifdef DUMA_USE_FRAMENO
    slot[1].frame           = 0;
#endif
    slot[1].filename        = __FILE__;
    slot[1].lineno          = __LINE__;
#endif
  }

  /*
   * Deny access to the free page, so that we will detect any software
   * that treads upon free memory.
   */
  Page_DenyAccess(slot[1].internalAddress, slot[1].internalSize);

  /*
   * Account for the two slot structures that we've used.
   */
  unUsedSlots = slotCount - 2;

#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
  if (duma_init_done)
#endif
    DUMA_RELEASE_SEMAPHORE();
#endif

#ifndef DUMA_EXPLICIT_INIT
  duma_init();
#endif
}


/*
 * allocateMoreSlots is called when there are only enough slot structures
 * left to support the allocation of a single malloc buffer.
 */
static void
allocateMoreSlots(void)
{
  size_t  newSize = _duma_allocListSize + DUMA_PAGE_SIZE;
  void *  newAllocation;
  void *  oldAllocation = _duma_allocList;

#ifndef DUMA_NO_LEAKDETECTION
  newAllocation = _duma_allocate( 1 /*=alignment*/, newSize, 0 /*=protectBelow*/, -1 /*=fillByte*/, 0 /*=protectAllocList*/, EFA_INT_ALLOC, DUMA_FAIL_NULL, __FILE__, __LINE__ );
#else
  newAllocation = _duma_allocate( 1 /*=alignment*/, newSize, 0 /*=protectBelow*/, -1 /*=fillByte*/, 0 /*=protectAllocList*/, EFA_INT_ALLOC, DUMA_FAIL_NULL);
#endif

  memcpy(newAllocation, _duma_allocList, _duma_allocListSize);
  memset(&(((char *)newAllocation)[_duma_allocListSize]), 0, DUMA_PAGE_SIZE);

  _duma_allocList = (struct _DUMA_Slot *)newAllocation;
  _duma_allocListSize = newSize;
  slotCount   += slotsPerPage;
  unUsedSlots += slotsPerPage;

#ifndef DUMA_NO_LEAKDETECTION
  _duma_deallocate( oldAllocation, 0 /*=protectAllocList*/, EFA_INT_DEALLOC, __FILE__, __LINE__ );
#else
  _duma_deallocate( oldAllocation, 0 /*=protectAllocList*/, EFA_INT_DEALLOC);
#endif
}


/*
 * This is the memory allocator. When asked to allocate a buffer, allocate
 * it in such a way that the end of the buffer is followed by an inaccessable
 * memory page. If software overruns that buffer, it will touch the bad page
 * and get an immediate segmentation fault. It's then easy to zero in on the
 * offending code with a debugger.
 *
 * There are a few complications. If the user asks for an odd-sized buffer,
 * we would have to have that buffer start on an odd address if the byte after
 * the end of the buffer was to be on the inaccessable page. Unfortunately,
 * there is lots of software that asks for odd-sized buffers and then
 * requires that the returned address be word-aligned, or the size of the
 * buffer be a multiple of the word size. An example are the string-processing
 * functions on Sun systems, which do word references to the string memory
 * and may refer to memory up to three bytes beyond the end of the string.
 * For this reason, I take the alignment requests to memalign() and valloc()
 * seriously, and 
 * 
 * DUMA wastes lots of memory. I do a best-fit allocator here
 * so that it won't waste even more. It's slow, but thrashing because your
 * working set is too big for a system's RAM is even slower. 
 */

void * _duma_allocate(size_t alignment, size_t userSize, int protectBelow, int fillByte, int protectAllocList, enum _DUMA_Allocator allocator, enum _DUMA_FailReturn fail  DUMA_PARAMLIST_FL)
{
  size_t            count;
  struct _DUMA_Slot * slot;
  struct _DUMA_Slot * fullSlot;
  struct _DUMA_Slot * emptySlots[2];
  duma_number         intAddr, userAddr, protAddr, endAddr;
  size_t            internalSize;

  DUMA_ASSERT( 0 != _duma_allocList );

  /* check userSize */
  if ( 0 == userSize && !DUMA_ALLOW_MALLOC_0 )
  {
    #ifndef DUMA_NO_LEAKDETECTION
      DUMA_Abort("Allocating 0 bytes, probably a bug: %s(%d)",
               filename, lineno);
    #else
      DUMA_Abort("Allocating 0 bytes, probably a bug.");
    #endif
  }

  /* check alignment */
  if ( alignment == 0 )
  {
    alignment = 1;
  }
  if ( (int)alignment != ((int)alignment & -(int)alignment) )
  {
    #ifndef DUMA_NO_LEAKDETECTION
      DUMA_Abort("Alignment (=%d) is not a power of 2 requested from %s(%d)",
               alignment, filename, lineno);
    #else
      DUMA_Abort("Alignment (=%d) is not a power of 2", alignment);
    #endif
  }

  /* count and show allocation, if requested */
  numAllocs++;
  if (DUMA_SHOW_ALLOC)
#ifndef DUMA_NO_LEAKDETECTION
    DUMA_Print("\nDUMA: Allocating %d bytes at %s(%d).", userSize, filename, lineno);
#else
    DUMA_Print("\nDUMA: Allocating %d bytes.", userSize);
#endif

  /*
   * If protectBelow is set, all addresses returned by malloc()
   * and company will be page-aligned.
   *
   * The internal size of the buffer is rounded up to the next alignment and page-size
   * boundary, and then we add another page's worth of memory for the dead page.
   */
  /* a bit tricky but no modulo and no if () */
  internalSize = ( (userSize + DUMA_PAGE_SIZE -1) & ~(DUMA_PAGE_SIZE -1) )
                 + DUMA_PAGE_SIZE;
  if ( alignment > DUMA_PAGE_SIZE )
    internalSize += alignment - DUMA_PAGE_SIZE;

  /*
   * These will hold the addresses of two empty Slot structures, that
   * can be used to hold information for any memory I create, and any
   * memory that I mark free.
   */
  fullSlot = emptySlots[0] = emptySlots[1] = 0;


  /*
   * The internal memory used by the allocator is currently
   * inaccessable, so that errant programs won't scrawl on the
   * allocator's arena. I'll un-protect it here so that I can make
   * a new allocation. I'll re-protect it before I return.
   */
  if ( protectAllocList )
  {
#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
    if (duma_init_done)
#endif
      DUMA_GET_SEMAPHORE();
#endif
    Page_AllowAccess(_duma_allocList, _duma_allocListSize);
  }

  /*
   * If I'm running out of empty slots, create some more before
   * I don't have enough slots left to make an allocation.
   */
  if ( DUMAAT_INTERNAL != _duma_allocDesc[allocator].type  &&  unUsedSlots < 7 )
    allocateMoreSlots();

  /*
   * Iterate through all of the slot structures. Attempt to find a slot
   * containing free memory of the exact right size. Accept a slot with
   * more memory than we want, if the exact right size is not available.
   * Find two slot structures that are not in use. We will need one if
   * we split a buffer into free and allocated parts, and the second if
   * we have to create new memory and mark it as free.
   *
   */
  for ( slot = _duma_allocList, count = slotCount ; count > 0; --count, ++slot )
  {
    /*
     * Windows needs special treatment, cause Page_Delete() needs exactly
     * the same memory region as Page_Create()!
     * Thus as a quick hack no memory management is done by DUMA.
     */
#if !defined(WIN32)
    if ( DUMAST_FREE == slot->state  &&  slot->internalSize >= internalSize )
    {
      if ( !fullSlot || slot->internalSize < fullSlot->internalSize )
      {
        fullSlot = slot;
        if ( slot->internalSize == internalSize )
          break;  /* All done; no empty slot needed in this case */
      }
    }
    else
#endif
    if ( DUMAST_EMPTY == slot->state )
    {
      if      ( !emptySlots[0] )    emptySlots[0] = slot;
      else if ( !emptySlots[1] )    emptySlots[1] = slot;
#if defined(WIN32)
      break;
#endif
    }
  }

  if ( !fullSlot )
  {
    /*
     * I get here if I haven't been able to find a free buffer
     * with all of the memory I need. I'll have to create more
     * memory. I'll mark it all as free, and then split it into
     * free and allocated portions later.
     */
    size_t  chunkSize;
    long    chunkSizekB;

#if defined(WIN32)
    chunkSize = internalSize;
#else
    chunkSize = MEMORY_CREATION_SIZE;

    if ( chunkSize < internalSize )
      chunkSize = internalSize;

    chunkSize = ( chunkSize + DUMA_PAGE_SIZE -1 ) & ~( DUMA_PAGE_SIZE -1 );
#endif
    chunkSizekB = (chunkSize+1023) >>10;


    /* Use up one of the empty slots to make the full slot. */
    if ( !emptySlots[0] )
      DUMA_Abort("Internal error in allocator: No empty slot 0.\n");
#if !defined(WIN32)
    if ( !emptySlots[1] )
      DUMA_Abort("Internal error in allocator: No empty slot 1.\n");
#endif

    fullSlot      = emptySlots[0];
    emptySlots[0] = emptySlots[1];

    /* reduce protected memory when we would exceed DUMA_MAX_ALLOC */
    if ( DUMA_MAX_ALLOC > 0  &&  sumAllocatedMem + chunkSizekB > DUMA_MAX_ALLOC )
      reduceProtectedMemory( chunkSizekB );

    fullSlot->internalAddress = Page_Create( chunkSize, 0/*= exitonfail*/ );
    if ( 0 == fullSlot->internalAddress  &&  0L != DUMA_PROTECT_FREE )
    {
      /* reduce as much protected memory as we need - or at least try so */
      reduceProtectedMemory( (chunkSize+1023) >>10 );
      /* simply try again */
      fullSlot->internalAddress = Page_Create( chunkSize, DUMA_MALLOC_FAILEXIT );
    }
    if ( fullSlot->internalAddress )
    {
      sumAllocatedMem          += ( (chunkSize +1023) >>10 );
      sumTotalAllocatedMem     += ( (chunkSize +1023) >>10 );
      fullSlot->internalSize    = chunkSize;
      fullSlot->state           = DUMAST_FREE;
      --unUsedSlots;
    }
  }

  if ( fullSlot->internalSize )
  {

#if !defined(WIN32)
    /*
     * If the buffer I've found is larger than I need, split it into
     * an allocated buffer with the exact amount of memory I need, and
     * a free buffer containing the surplus memory.
     */
    if ( fullSlot->internalSize > internalSize )
    {
      /* copy and adjust contents for free slot */
      *emptySlots[0]                 = *fullSlot;
      emptySlots[0]->internalAddress = (char *)emptySlots[0]->internalAddress
                                     + internalSize;
      emptySlots[0]->internalSize   -= internalSize;
      emptySlots[0]->userAddress     = emptySlots[0]->internalAddress;
      emptySlots[0]->userSize        = emptySlots[0]->internalSize;

      /* adjust size of fullSlot */
      fullSlot->internalSize         = internalSize;

      --unUsedSlots;
    }
#endif

    if ( !protectBelow )
    {
      /*
       * Arrange the buffer so that it is followed by an inaccessable
       * memory page. A buffer overrun that touches that page will
       * cause a segmentation fault.
       * internalAddr <= userAddr < protectedAddr
       */

      /* Figure out what address to give the user. */
      intAddr  = (duma_number)fullSlot->internalAddress;
      endAddr  = intAddr + internalSize;
      userAddr = ( intAddr  + internalSize - DUMA_PAGE_SIZE - userSize )
                & ~(alignment -1); 
      protAddr = ( userAddr + userSize     + DUMA_PAGE_SIZE -1)
                & ~(DUMA_PAGE_SIZE -1);

      /* DUMA_ASSERT(intAddr <= userAddr && intAddr < protAddr ); */

      /* Set up the "live" page(s). */
      Page_AllowAccess( (char*)intAddr, protAddr - intAddr );
      /* Set up the "dead" page(s). */
      Page_DenyAccess( (char*)protAddr, endAddr - protAddr );
    }
    else /* if (protectBelow) */
    {
      /*
       * Arrange the buffer so that it is preceded by an inaccessable
       * memory page. A buffer underrun that touches that page will
       * cause a segmentation fault.
       */
      /* Figure out what address to give the user. */
      intAddr  = (duma_number)fullSlot->internalAddress;
      endAddr  = intAddr + internalSize;
      userAddr = ( intAddr + DUMA_PAGE_SIZE + alignment -1)
                & ~(alignment -1);
      protAddr = ( userAddr & ~(DUMA_PAGE_SIZE -1) ) - DUMA_PAGE_SIZE;

      /* DUMA_ASSERT(intAddr < userAddr && intAddr <= protAddr ); */

      /* Set up the "live" page(s). userAddr == protAddr + DUMA_PAGE_SIZE ! */
      Page_AllowAccess( (char*)userAddr, internalSize - (userAddr - protAddr) );
      /* Set up the "dead" page(s). */
      Page_DenyAccess( (char*)intAddr, userAddr - intAddr );
    }

    /* => userAddress = internalAddress + DUMA_PAGE_SIZE */
    fullSlot->userAddress = (char*)userAddr;
    fullSlot->protAddress = (char*)protAddr;
    fullSlot->userSize    = userSize;
    fullSlot->state       = DUMAST_IN_USE;
    fullSlot->allocator   = allocator;
  #ifndef DUMA_NO_LEAKDETECTION
    fullSlot->fileSource  = DUMAFS_ALLOCATION;
  #ifdef DUMA_USE_FRAMENO
    fullSlot->frame       = frameno;
  #endif
    fullSlot->filename    = (char*)filename;
  #ifdef DUMA_EXPLICIT_INIT
    /* mark allocations from standard libraries
     * before duma_init() is called with lineno = -1
     * to allow special treatment in leak_checking
     */
    fullSlot->lineno      = (duma_init_done) ? lineno : -1;
  #else
    fullSlot->lineno      = lineno;
  #endif
  #endif

    /* initialise no mans land of slot */
    _duma_init_slack( fullSlot );

  } /* end if ( fullSlot->internalSize ) */


  /*
   * Make the pool's internal memory inaccessable, so that the program
   * being debugged can't stomp on it.
   */
  if ( protectAllocList )
  {
    Page_DenyAccess(_duma_allocList, _duma_allocListSize);
#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
    if (duma_init_done)
#endif
      DUMA_RELEASE_SEMAPHORE();
#endif
  }

  /* Fill the memory if it was specified to do so. */
  if ( ((char*)userAddr) && fillByte != -1 )
    memset( (char*)userAddr, fillByte, userSize);

  return (char*)userAddr;
}



void   _duma_deallocate(void * address, int protectAllocList, enum _DUMA_Allocator allocator  DUMA_PARAMLIST_FL)
{
  struct _DUMA_Slot   * slot;
  long                internalSizekB;

  if ( 0 == _duma_allocList )
    DUMA_Abort("free() called before first malloc().");

  if ( 0 == address )
    return;

  if ( protectAllocList )
  {
#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
    if (duma_init_done)
#endif
      DUMA_GET_SEMAPHORE();
#endif
    Page_AllowAccess(_duma_allocList, _duma_allocListSize);
  }

  if ( !(slot = slotForUserAddress(address)) )
  {
    if ( (slot = nearestSlotForUserAddress(address)) )
    {
    #ifndef DUMA_NO_LEAKDETECTION
      if ( DUMAFS_ALLOCATION == slot->fileSource )
        DUMA_Abort("free(%a): address not from DUMA or already freed. Address may be corrupted from %a allocated from %s(%d)",
                 address, slot->userAddress, slot->filename, slot->lineno);
      else if ( DUMAFS_DEALLOCATION == slot->fileSource )
        DUMA_Abort("free(%a): address not from DUMA or already freed. Address may be corrupted from %a deallocated at %s(%d)",
                 address, slot->userAddress, slot->filename, slot->lineno);
      else
    #endif
        DUMA_Abort("free(%a): address not from DUMA or already freed. Address may be corrupted from %a.",
                 address, slot->userAddress);
    }
    else
      DUMA_Abort("free(%a): address not from DUMA or already freed.", address);
  }

  if ( DUMAST_ALL_PROTECTED == slot->state || DUMAST_BEGIN_PROTECTED == slot->state )
  {
  #ifndef DUMA_NO_LEAKDETECTION
    if ( DUMAFS_ALLOCATION == slot->fileSource )
      DUMA_Abort("free(%a): memory already freed. allocated from %s(%d)",
               address, slot->filename, slot->lineno);
    else if ( DUMAFS_DEALLOCATION == slot->fileSource )
      DUMA_Abort("free(%a): memory already freed at %s(%d)",
               address, slot->filename, slot->lineno);
    else
  #endif
      DUMA_Abort("free(%a): memory already freed.", address);
  }
  else if ( _duma_allocDesc[slot->allocator].type != _duma_allocDesc[allocator].type )
  {
  #ifndef DUMA_NO_LEAKDETECTION
    if ( DUMAFS_ALLOCATION == slot->fileSource )
      DUMA_Abort("Free mismatch: allocator '%s' used  at %s(%d)\n  but  deallocator '%s' called at %s(%d)!",
               _duma_allocDesc[slot->allocator].name, slot->filename, slot->lineno,
               _duma_allocDesc[allocator].name, filename, lineno );
    else if ( DUMAFS_DEALLOCATION == slot->fileSource )
      DUMA_Abort("Free mismatch: allocator '%s' used \nbut deallocator '%s' called at %s(%d)!",
               _duma_allocDesc[slot->allocator].name,
               _duma_allocDesc[allocator].name, filename, lineno );
    else
  #endif
    DUMA_Abort("Free mismatch: allocator '%s' used  but  deallocator '%s' called!",
             _duma_allocDesc[slot->allocator].name, _duma_allocDesc[allocator].name );
  }

  /* count and show deallocation, if requested */
  numDeallocs++;
  if (DUMA_SHOW_ALLOC)
#ifndef DUMA_NO_LEAKDETECTION
    DUMA_Print("\nDUMA: Freeing %d bytes at %s(%d) (Allocated from %s(%d)).", slot->userSize, filename, lineno, slot->filename, slot->lineno);
#else
    DUMA_Print("\nDUMA: Freeing %d bytes.", slot->userSize);
#endif

  /* CHECK INTEGRITY OF NO MANS LAND */
  _duma_check_slack( slot );

  if ( DUMA_FREE_ACCESS )
  {
    volatile char *start = slot->userAddress;
    volatile char *cur;
    for (cur = (char*)slot->userAddress+slot->userSize; --cur >= start; )
    {
      char c = *cur;
      *cur = c-1;
      *cur = c;
    }
  }

  if ( DUMA_FREE_WIPES )
    memset(slot->userAddress, DUMA_FILL, slot->userSize);

  internalSizekB = (slot->internalSize+1023) >>10;

  /* protect memory, that nobody can access it */
  /* Free as much protected memory, that we can protect this one */
    /* is there need? and is there a way to free such much? */
  if ( DUMA_PROTECT_FREE > 0L
      && sumProtectedMem  + internalSizekB >  DUMA_PROTECT_FREE
      &&                    internalSizekB <  DUMA_PROTECT_FREE
      && sumProtectedMem >= internalSizekB
     )
    reduceProtectedMemory( internalSizekB );

  if (   ( EFA_INT_ALLOC != slot->allocator )
      && ( DUMA_PROTECT_FREE < 0L
          || ( DUMA_PROTECT_FREE > 0L
              && sumProtectedMem + internalSizekB <= DUMA_PROTECT_FREE
         )   )
     )
  {
    slot->state = DUMAST_ALL_PROTECTED;
    Page_DenyAccess(slot->internalAddress, slot->internalSize);
    sumProtectedMem += internalSizekB;

    #ifndef DUMA_NO_LEAKDETECTION
      if ( lineno )
      {
        slot->fileSource  = DUMAFS_DEALLOCATION;
        slot->filename    = (char*)filename;
        slot->lineno      = lineno;
      }
    #endif
  }
  else
  {
    /* free all the memory */
    Page_Delete(slot->internalAddress, slot->internalSize);
    sumAllocatedMem -= internalSizekB;
    /* free slot and userAddr */
    slot->internalAddress = slot->userAddress = 0;
    slot->internalSize    = slot->userSize    = 0;
    slot->state           = DUMAST_EMPTY;
    slot->allocator       = EFA_INT_ALLOC;
    #ifndef DUMA_NO_LEAKDETECTION
    slot->fileSource      = DUMAFS_EMPTY;
    #ifdef DUMA_USE_FRAMENO
      slot->frame         = 0;
    #endif
      slot->filename      = 0;
      slot->lineno        = 0;
    #endif
  }

  if ( protectAllocList )
  {
    Page_DenyAccess(_duma_allocList, _duma_allocListSize);
#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
    if (duma_init_done)
#endif
      DUMA_RELEASE_SEMAPHORE();
#endif
  }
}


/*********************************************************/

void * _duma_malloc(size_t size  DUMA_PARAMLIST_FL)
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  return _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_MALLOC, DUMA_FAIL_ENV  DUMA_PARAMS_FL);
}


void * _duma_calloc(size_t nelem, size_t elsize  DUMA_PARAMLIST_FL)
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  return _duma_allocate(DUMA_ALIGNMENT, nelem * elsize, DUMA_PROTECT_BELOW, 0 /*=fillByte*/, 1 /*=protectAllocList*/, EFA_CALLOC, DUMA_FAIL_ENV  DUMA_PARAMS_FL);
}


void   _duma_free(void * baseAdr  DUMA_PARAMLIST_FL)
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  _duma_deallocate(baseAdr, 1 /*=protectAllocList*/, EFA_FREE  DUMA_PARAMS_FL);
}


void * _duma_memalign(size_t alignment, size_t size  DUMA_PARAMLIST_FL)
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  return _duma_allocate(alignment, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_MEMALIGN, DUMA_FAIL_ENV  DUMA_PARAMS_FL);
}


void * _duma_realloc(void * oldBuffer, size_t newSize  DUMA_PARAMLIST_FL)
{
  void * ptr;
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
  if (duma_init_done)
#endif
    DUMA_GET_SEMAPHORE();
#endif
  Page_AllowAccess(_duma_allocList, _duma_allocListSize);

  ptr = _duma_allocate(DUMA_ALIGNMENT, newSize, DUMA_PROTECT_BELOW, -1 /*=fillByte*/, 0 /*=protectAllocList*/, EFA_REALLOC, DUMA_FAIL_ENV  DUMA_PARAMS_FL);

  if ( ptr && oldBuffer )
  {
    struct _DUMA_Slot * slot = slotForUserAddress(oldBuffer);

    if ( slot == 0 )
      DUMA_Abort("realloc(%a, %d): address not from malloc().", oldBuffer, newSize);

    if ( newSize > slot->userSize )
    {
      memcpy( ptr, oldBuffer, slot->userSize );
      memset( (char*)ptr + slot->userSize, 0, newSize - slot->userSize );
    }
    else if ( newSize > 0 )
      memcpy(ptr, oldBuffer, newSize);

    _duma_deallocate(oldBuffer, 0 /*=protectAllocList*/, EFA_REALLOC  DUMA_PARAMS_FL);
  }

  Page_DenyAccess(_duma_allocList, _duma_allocListSize);
#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
  if (duma_init_done)
#endif
    DUMA_RELEASE_SEMAPHORE();
#endif
  return ptr;
}


void * _duma_valloc(size_t size  DUMA_PARAMLIST_FL)
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  return _duma_allocate(DUMA_PAGE_SIZE, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_VALLOC, DUMA_FAIL_ENV  DUMA_PARAMS_FL);
}


char * _duma_strdup(const char * str  DUMA_PARAMLIST_FL)
{
  size_t size;
  char * dup;
  unsigned i;

  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */

  size = 0;
  while (str[size]) ++size;

  dup = _duma_allocate(DUMA_PAGE_SIZE, size +1, DUMA_PROTECT_BELOW, -1 /*=fillByte*/, 1 /*=protectAllocList*/, EFA_STRDUP, DUMA_FAIL_ENV  DUMA_PARAMS_FL);

  if (dup)                    /* if successful */
    for (i=0; i<=size; ++i)   /* copy string */
      dup[i] = str[i];

  return dup;
}


void * _duma_memcpy(void *dest, const void *src, size_t size  DUMA_PARAMLIST_FL)
{
  char       * d = (char *)dest;
  const char * s = (const char *)src;
  unsigned i;

  if ( (s < d  &&  d < s + size) || (d < s  &&  s < d + size) )
    DUMA_Abort("memcpy(%a, %a, %d): memory regions overlap.", dest, src, size);

  for (i=0; i<size; ++i)
    d[i] = s[i];

  return dest;
}


char * _duma_strcpy(char *dest, const char *src  DUMA_PARAMLIST_FL)
{
  unsigned i;
  size_t size = strlen(src) +1;

  if ( src < dest  &&  dest < src + size )
    DUMA_Abort("strcpy(%a, %a): memory regions overlap.", dest, src);

  for (i=0; i<size; ++i)
    dest[i] = src[i];

  return dest;
}


char * _duma_strncpy(char *dest, const char *src, size_t size  DUMA_PARAMLIST_FL)
{
  size_t srcsize;
  unsigned i;

  if ( size > 0  &&  src < dest  &&  dest < src + size )
    DUMA_Abort("strncpy(%a, %a, %d): memory regions overlap.", dest, src, size);

  /* calculate number of characters to copy from src to dest */
  srcsize = strlen(src) +1;
  if ( srcsize > size )
    srcsize = size;

  /* copy src to dest */
  for (i=0; i<srcsize; ++i)
    dest[i] = src[i];

  /* fill rest with '\0' character */
  for (   ; i<size;   ++i)
    dest[i] = 0;

  return dest;
}


char * _duma_strcat(char *dest, const char *src  DUMA_PARAMLIST_FL)
{
  unsigned i;
  size_t destlen = strlen(dest);
  size_t srcsize = strlen(src)  +1;

  if ( src < dest +destlen  &&  dest + destlen < src + srcsize )
    DUMA_Abort("strcat(%a, %a): memory regions overlap.", dest, src);

  for (i=0; i<srcsize; ++i)
    dest[destlen+i] = src[i];

  return dest;
}


char * _duma_strncat(char *dest, const char *src, size_t size  DUMA_PARAMLIST_FL)
{
  unsigned i;
  size_t destlen, srclen;

  /* do nothing, when size not > 0 */
  if ( size <= 0 )
    return dest;

  /* calculate number of characters to copy from src to dest */
  destlen = strlen(dest);
  srclen  = strlen(src);
  if ( srclen > size )
    srclen = size;

  if ( src < dest +destlen  &&  dest + destlen < src + srclen +1 )
    DUMA_Abort("strncat(%a, %a, %d): memory regions overlap.", dest, src, size);

  /* copy up to size characters from src to dest */
  for (i=0; i<srclen; ++i)
    dest[destlen+i] = src[i];
  /* append single '\0' character */
  dest[destlen+srclen] = 0;

  return dest;
}


/*********************************************************/


#ifndef DUMA_NO_GLOBAL_MALLOC_FREE

/*
 * define global functions for malloc(), free(), ..
 */

#ifdef _MSC_VER
/* define these functions as non-intrinsic */
#pragma function( memcpy, strcpy, strcat )
#endif


void * malloc(size_t size)
{
  return _duma_malloc(size  DUMA_PARAMS_UK);
}


void * calloc(size_t nelem, size_t elsize)
{
  return _duma_calloc(nelem, elsize  DUMA_PARAMS_UK);
}


void   free(void * address)
{
  _duma_free(address  DUMA_PARAMS_UK);
}


void * memalign(size_t alignment, size_t size)
{
  return _duma_memalign(alignment, size  DUMA_PARAMS_UK);
}


void * realloc(void * oldBuffer, size_t newSize)
{
  return _duma_realloc(oldBuffer, newSize  DUMA_PARAMS_UK);
}


void * valloc(size_t size)
{
  return _duma_valloc(size  DUMA_PARAMS_UK);
}


char * strdup(const char * str)
{
  return _duma_strdup(str  DUMA_PARAMS_UK);
}


void * memcpy(void *dest, const void *src, size_t size)
{
  return _duma_memcpy(dest, src, size  DUMA_PARAMS_UK);
}


char * strcpy(char *dest, const char *src)
{
  return _duma_strcpy(dest, src  DUMA_PARAMS_UK);
}


char * strncpy(char *dest, const char *src, size_t size)
{
  return _duma_strncpy(dest, src, size  DUMA_PARAMS_UK);
}


char * strcat(char *dest, const char *src)
{
  return _duma_strcat(dest, src  DUMA_PARAMS_UK);
}


char * strncat(char *dest, const char *src, size_t size)
{
  return _duma_strncat(dest, src, size  DUMA_PARAMS_UK);
}


#endif /* DUMA_NO_GLOBAL_MALLOC_FREE */





#ifndef DUMA_NO_LEAKDETECTION

/* *********************************************************
 *
 *  void  DUMA_newFrame(void);
 *
 ***********************************************************/

void  DUMA_newFrame(void)
{
#ifdef DUMA_USE_FRAMENO
  ++frameno;
#endif
}


/* *********************************************************
 *
 *  void  DUMA_delFrame(void);
 *
 ***********************************************************/

void  DUMA_delFrame(void)
{
#ifdef DUMA_USE_FRAMENO
  if (-1 != frameno)
  {
#endif
    struct _DUMA_Slot * slot      = _duma_allocList;
    size_t            count     = slotCount;
    int               nonFreed  = 0;

#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
    if (duma_init_done)
#endif
      DUMA_GET_SEMAPHORE();
#endif
    Page_AllowAccess(_duma_allocList, _duma_allocListSize);

    for ( ; count > 0; --count, ++slot )
    {
      if (    DUMAST_IN_USE == slot->state
        #ifdef DUMA_USE_FRAMENO
           && frameno == slot->frame
        #endif
           && EFA_INT_ALLOC != slot->allocator
        #ifdef DUMA_EXPLICIT_INIT
           && -1 != slot->lineno
        #endif
         )
      {
        DUMA_Print("\nDUMA: ptr=0x%a size=%d alloced from %s(%d) not freed",
          slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        ++nonFreed;
      }
    }
    if (nonFreed)
      DUMA_Abort("DUMA_delFrame(): Found non free'd pointers.\n");

    Page_DenyAccess(_duma_allocList, _duma_allocListSize);
#ifndef DUMA_NO_THREAD_SAFETY
#ifdef DUMA_EXPLICIT_INIT
    if (duma_init_done)
#endif
      DUMA_RELEASE_SEMAPHORE();
#endif

  #ifdef DUMA_USE_FRAMENO
    --frameno;
  }
  #endif
  if (DUMA_SHOW_ALLOC)
    DUMA_Print("\nDUMA: DUMA_delFrame(): Processed %d allocations and %d deallocations in total.\n", numAllocs, numDeallocs);
}

#endif /* end ifndef DUMA_NO_LEAKDETECTION */

#endif /* ifndef DUMA_NO_DUMA */

