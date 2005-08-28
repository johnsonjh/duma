
/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
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
 * better than Electric Fence: "Purify" from Purify Systems, and that's only
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

#ifndef EF_NO_EFENCE

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

#include "efence.h"
#include "efenceint.h"
#include "print.h"
#include "sem_inc.h"
#include "paging.h"

static const char  version[] = "\n"
"Electric Fence 2.4.16\n"
"Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>\n"
"Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH\n";


static const char unknown_file[] =
  "UNKNOWN (use #include \"efence.h\")";


#ifndef EF_NO_LEAKDETECTION
#define EF_PARAMLIST_FL       , const char * filename, int lineno
#define EF_PARAMS_FL          , filename, lineno
#define EF_PARAMS_UK          , unknown_file, 0
#else
#define EF_PARAMLIST_FL
#define EF_PARAMS_FL
#define EF_PARAMS_UK
#endif


/*
 * MEMORY_CREATION_SIZE is the amount of memory to get from the operating
 * system at one time. We'll break that memory down into smaller pieces for
 * malloc buffers. One megabyte is probably a good value.
 */
#define      MEMORY_CREATION_SIZE  1024 * 1024

enum _EF_SlotState
{
    EFST_EMPTY            /* slot not in use */
  , EFST_FREE             /* internal memory reserved, unused by user */
  , EFST_IN_USE           /* memory in use by allocator; see following enum AllocType */
  , EFST_ALL_PROTECTED    /* memory no more used by allocator; memory is not deallocated but protected */
  , EFST_BEGIN_PROTECTED  /* most memory deallocated, but not page covering userAddress:
                           * slot holds userAddress, userSize and allocator.
                           */
};

enum _EF_Slot_FileSource
{
    EFFS_EMPTY            /* no filename, lineno */
  , EFFS_ALLOCATION       /* filename, lineno from allocation */
  , EFFS_DEALLOCATION     /* filename, lineno from deallocation */
};

/*
 * Struct Slot contains all of the information about a malloc buffer except
 * for the contents of its memory.
 */
struct _EF_Slot
{
  void            * internalAddress;
  void            * userAddress;
  void            * protAddress;
  size_t            internalSize;
  size_t            userSize;

#if 0
  /* just for checking compiler warnings / errors */
  enum _EF_SlotState        state;
  enum _EF_Allocator        allocator;
  #ifndef EF_NO_LEAKDETECTION
  enum _EF_Slot_FileSource  fileSource;
  #endif
#else
  /* save (some) space in production */
  unsigned short    state       :16;
  #ifdef EF_NO_LEAKDETECTION
  unsigned short    allocator   :16;
  #else
  unsigned short    allocator   :8;
  unsigned short    fileSource  :8;
  #endif
#endif

#ifndef EF_NO_LEAKDETECTION
#ifdef EF_USE_FRAMENO
  int               frame;
#endif
  char            * filename;   /* filename of allocation */
  int               lineno;     /* linenumber of allocation */
#endif

#ifdef EF_EXPLICIT_INIT
  int               slackfill;
#endif
};

enum _EF_AllocType
{
    EFAT_INTERNAL
  , EFAT_MALLOC
  , EFAT_NEW_ELEM
  , EFAT_NEW_ARRAY
};

static struct _EF_AllocDesc
{
  char                * name;
  enum _EF_AllocType    type;
}
_eff_allocDesc[] =
{
    { "efence allocate()"   , EFAT_INTERNAL  }
  , { "efence deallocate()" , EFAT_INTERNAL  }
  , { "malloc()"            , EFAT_MALLOC    }
  , { "calloc()"            , EFAT_MALLOC    }
  , { "free()"              , EFAT_MALLOC    }
  , { "memalign()"          , EFAT_MALLOC    }
  , { "realloc()"           , EFAT_MALLOC    }
  , { "valloc()"            , EFAT_MALLOC    }
  , { "strdup()"            , EFAT_MALLOC    }
  , { "new (element)"       , EFAT_NEW_ELEM  }
  , { "delete (element)"    , EFAT_NEW_ELEM  }
  , { "new[] (array)"       , EFAT_NEW_ARRAY }
  , { "[]delete (array)"    , EFAT_NEW_ARRAY }
};

#ifndef EF_NO_LEAKDETECTION
#ifdef EF_USE_FRAMENO
static int    frameno = 0;
#endif
#endif

/*
 * EF_DISABLE_BANNER is a global variable used to control whether
 * Electric Fence prints its usual startup message. If the value is
 * -1, it will be set from the environment default to 0 at run time.
 */
static int    EF_DISABLE_BANNER = -1;

/*
 * EF_ALIGNMENT is a global variable used to control the default alignment
 * of buffers returned by malloc(), calloc(), and realloc(). It is all-caps
 * so that its name matches the name of the environment variable that is used
 * to set it. This gives the programmer one less name to remember.
 */
int           EF_ALIGNMENT = sizeof(int);

/*
 * EF_PROTECT_BELOW is used to modify the behavior of the allocator. When
 * its value is non-zero, the allocator will place an inaccessable page
 * immediately _before_ the malloc buffer in the address space, instead
 * of _after_ it. Use this to detect malloc buffer under-runs, rather than
 * over-runs. It won't detect both at the same time, so you should test your
 * software twice, once with this value clear, and once with it set.
 */
int           EF_PROTECT_BELOW = 0;

/*
 * EF_FILL is set to 0-255 if Electric Fence should fill all new allocated
 * memory with the specified value. Set to -1 when Electric Fence should not
 * initialise allocated memory.
 * default is set to initialise with 255, cause many programs rely on
 * initialisation to 0!
 */
int           EF_FILL = 255;

/*
 * EF_SLACKFILL is set to 0-255. The slack / no mans land of all new allocated
 * memory is filled with the specified value.
 * default is set to initialise with 0xAA (=binary 10101010)
 * initialisation to 0!
 */
static int    EF_SLACKFILL = 0xAA;

/*
 * EF_PROTECT_FREE is used to control the disposition of memory that is
 * released using free(). It is all-caps so that its name
 * matches the name of the environment variable that is used to set it.
 * If its value is non-zero, memory released by free is made inaccessable.
 * Any software that touches free memory will then get a segmentation fault.
 * Depending on your application and your resources you may tell
 * Electric Fence not to use this memory ever again by setting a negative
 * value f.e. -1.
 * You can tell Electric Fence to limit the sum of protected memory by setting
 * a positive value, which is interpreted in kB.
 * If its value is zero, freed memory will be available for reallocation,
 * but will still be inaccessable until it is reallocated.
 */
static long   EF_PROTECT_FREE = -1L;

/*
 * EF_MAX_ALLOC is used to control the maximum memory print of the program
 * in total: When the sum of allocated and protected memory would exceed
 * this value in kB, the protected memory is freed/deleted.
 */
static long   EF_MAX_ALLOC = -1L;

/*
 * EF_ALLOW_MALLOC_0 is set if Electric Fence is to allow malloc(0). I
 * trap malloc(0) by default because it is a common source of bugs.
 * But you should know the allocation with size 0 is ANSI conform.
 */
static int    EF_ALLOW_MALLOC_0 = 1;

/*
 * EF_MALLOC_FAILEXIT controls the behaviour of Electric Fence when
 * malloc() fails and would return NULL. But most applications don't
 * check the return value for errors ... so
 * default to Exit on Fail
 */
static int    EF_MALLOC_FAILEXIT = 1;

/*
 * EF_FREE_ACCESS is set if Electric Fence is to write access memory before
 * freeing it. This makes easier using watch expressions in debuggers as the
 * process is interrupted even if the memory is going to be freed.
 */
static int    EF_FREE_ACCESS = 0;

/*
 * EF_FREE_WIPES is set if Electric Fence is to wipe the memory content
 * of freed blocks. This makes it easier to check if memory is freed or
 * not
 */
static int    EF_FREE_WIPES = 0;

/*
 * EF_SHOW_ALLOC is set if Electric Fence is to print all allocations
 * and deallocations to the console. Although this generates a lot
 * of messages, the option can be useful to detect inefficient code
 * containing many allocations / deallocations
 */
static int    EF_SHOW_ALLOC = 0;

/*
 * _ef_allocList points to the array of slot structures used to manage the
 * malloc arena.
 */
struct _EF_Slot * _ef_allocList = 0;

/*
 * _ef_allocListSize is the size of the allocation list. This will always
 * be a multiple of the page size.
 */
static size_t _ef_allocListSize = 0;

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
 * internal variable: is ef_init() already done
 */
static int ef_init_done = 0;


/*
 * include helper functions
 */
#include "ef_hlp.h"



void _eff_assert(const char * exprstr, const char * filename, int lineno)
{
  int *pcAddr = 0;
  EF_Print("\nElectricFence: EF_ASSERT(%s) failed at\n%s(%d)\n", exprstr, filename, lineno );
  /* this is "really" bad, but it works. assert() from assert.h system header
   * stops only the current thread but the program goes on running under MS Visual C++.
   * This way the program definitely halts.
   */
  while (1)
    *pcAddr++ = 0;
}


#ifndef EF_EXPLICIT_INIT
static
#endif
void ef_init(void)
{
  char            * string;
  void            * testAlloc;

  if ( (string = getenv("EF_DISABLE_BANNER")) != 0 )
    EF_DISABLE_BANNER = (atoi(string) != 0);
  if ( !EF_DISABLE_BANNER )
    EF_Print(version);

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
  if ( (string = getenv("EF_ALIGNMENT")) != 0 )
    EF_ALIGNMENT = (size_t)atoi(string);

  /*
   * See if the user wants to protect the address space below a buffer,
   * rather than that above a buffer.
   */
  if ( (string = getenv("EF_PROTECT_BELOW")) != 0 )
    EF_PROTECT_BELOW = (atoi(string) != 0);

  /*
   * See if the user wants to protect memory that has been freed until
   * the program exits, rather than until it is re-allocated.
   * =-1 protect as much free'd memory as possible
   * =0 do not protect free'd memory
   * =N protect memory up to N kB
   */
  if ( (string = getenv("EF_PROTECT_FREE")) != 0 )
    EF_PROTECT_FREE = atol(string);

  /*
   * See if the user has a memory usage limit. This controls the maximum
   * memory print of the program in total: The sum of allocated and protected
   * memory won't exceed this limit.
   * =-1 use as much memory as possible
   * =N limit total memory usage to N kB
   */
  if ( (string = getenv("EF_MAX_ALLOC")) != 0 )
    EF_MAX_ALLOC = atol(string);

  /*
   * See if the user wants to allow malloc(0).
   */
  if ( (string = getenv("EF_ALLOW_MALLOC_0")) != 0 )
    EF_ALLOW_MALLOC_0 = (atoi(string) != 0);

  /*
   * See if the user wants to exit on malloc() failure
   */
  if ( (string = getenv("EF_MALLOC_FAILEXIT")) != 0 )
    EF_MALLOC_FAILEXIT = (atoi(string) != 0);

  /*
   * See if the user wants to write access freed memory
   */
  if ( (string = getenv("EF_FREE_ACCESS")) != 0 )
    EF_FREE_ACCESS = (atoi(string) != 0);

  /*
   * See if the user wants us to wipe out freed memory
   */
  if ( (string = getenv("EF_FREE_WIPES")) != 0 )
    EF_FREE_WIPES = (atoi(string) != 0);

  /*
   * Check if we should be filling new memory with a value.
   */
  if ( (string = getenv("EF_FILL")) != 0)
  {
    EF_FILL = atoi(string);
    if ( -1 != EF_FILL )
      EF_FILL &= 255;
  }

  /*
   * Check with which value the memories no mans land is filled
   */
  if ( (string = getenv("EF_SLACKFILL")) != 0)
    EF_SLACKFILL = atoi(string);
  EF_SLACKFILL &= 255;

  /*
   * See if the user wants to see allocations / frees
   */
  if ( (string = getenv("EF_SHOW_ALLOC")) != 0 )
    EF_SHOW_ALLOC = (atoi(string) != 0);


  /*
   * Register atexit()
   */
#ifndef EF_NO_LEAKDETECTION
  EF_Print("\nElectricFence: Registering with atexit().\n"
#ifdef WIN32
             "ElectricFence: If this hangs, change the library load order with EF_EXPLICIT_INIT.\n");
#else
             "ElectricFence: If this hangs, change the library load order with EF_EXPLICIT_INIT or LD_PRELOAD.\n");
#endif
  if ( atexit( EF_delFrame ) )
    EF_Abort("Cannot register exit function.\n");
  EF_Print("ElectricFence: Registration was successful.\n");
#endif

  /* initialize semaphoring */
  EF_INIT_SEMAPHORE();

#ifndef EF_NO_GLOBAL_MALLOC_FREE
  /*
   * Check whether malloc and free is available
   */
  testAlloc = malloc(123);
  if (numAllocs == 0)
    EF_Abort("malloc() is not bound to efence.\nElectricFence Aborting: Preload lib with 'LD_PRELOAD=libefence.so <prog>'.\n");

  free(testAlloc);
  if (numDeallocs == 0)
    EF_Abort("free() is not bound to efence.\nElectricFence Aborting: Preload lib with 'LD_PRELOAD=libefence.so <prog>'.\n");
#endif

  ef_init_done = 1;
}


/*
 * _eff_init sets up the memory allocation arena and the run-time
 * configuration information.
 */
void _eff_init(void)
{
  size_t            size = MEMORY_CREATION_SIZE;
  struct _EF_Slot * slot;

#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
  if (ef_init_done)
#endif
  EF_GET_SEMAPHORE();
#endif

  /*
   * Figure out how many Slot structures to allocate at one time.
   */
  slotCount = slotsPerPage = EF_PAGE_SIZE / sizeof(struct _EF_Slot);
  _ef_allocListSize = EF_PAGE_SIZE;

  if ( size < _ef_allocListSize )
    size = _ef_allocListSize;

  size = ( size + EF_PAGE_SIZE -1 ) & ~( EF_PAGE_SIZE -1 );

  /*
   * Allocate memory, and break it up into two malloc buffers. The
   * first buffer will be used for Slot structures, the second will
   * be marked free.
   */
  slot = _ef_allocList = (struct _EF_Slot *)Page_Create(size, 1/*=exitonfail*/);
  memset((char *)_ef_allocList, 0, _ef_allocListSize);

  /* enter _ef_allocList as slot to allow call to free() when doing allocateMoreSlots() */
  slot[0].internalAddress   = slot[0].userAddress = _ef_allocList;
  slot[0].internalSize      = slot[0].userSize    = _ef_allocListSize;
  slot[0].state             = EFST_IN_USE;
  slot[0].allocator         = EFA_INT_ALLOC;
#ifndef EF_NO_LEAKDETECTION
  slot[0].fileSource        = EFFS_ALLOCATION;
#ifdef EF_USE_FRAMENO
  slot[0].frame             = 0;
#endif
  slot[0].filename          = __FILE__;
  slot[0].lineno            = __LINE__;
#endif
  if ( size > _ef_allocListSize )
  {
    slot[1].internalAddress = slot[1].userAddress
                            = ((char *)slot[0].internalAddress) + slot[0].internalSize;
    slot[1].internalSize    = slot[1].userSize
                            =   size - slot[0].internalSize;
    slot[1].state           = EFST_FREE;
    slot[1].allocator       = EFA_INT_ALLOC;
#ifndef EF_NO_LEAKDETECTION
    slot[1].fileSource      = EFFS_ALLOCATION;
#ifdef EF_USE_FRAMENO
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

#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
  if (ef_init_done)
#endif
    EF_RELEASE_SEMAPHORE();
#endif

#ifndef EF_EXPLICIT_INIT
  ef_init();
#endif
}


/*
 * allocateMoreSlots is called when there are only enough slot structures
 * left to support the allocation of a single malloc buffer.
 */
static void
allocateMoreSlots(void)
{
  size_t  newSize = _ef_allocListSize + EF_PAGE_SIZE;
  void *  newAllocation;
  void *  oldAllocation = _ef_allocList;

#ifndef EF_NO_LEAKDETECTION
  newAllocation = _eff_allocate( 1 /*=alignment*/, newSize, 0 /*=protectBelow*/, -1 /*=fillByte*/, 0 /*=protectAllocList*/, EFA_INT_ALLOC, EF_FAIL_NULL, __FILE__, __LINE__ );
#else
  newAllocation = _eff_allocate( 1 /*=alignment*/, newSize, 0 /*=protectBelow*/, -1 /*=fillByte*/, 0 /*=protectAllocList*/, EFA_INT_ALLOC, EF_FAIL_NULL);
#endif

  memcpy(newAllocation, _ef_allocList, _ef_allocListSize);
  memset(&(((char *)newAllocation)[_ef_allocListSize]), 0, EF_PAGE_SIZE);

  _ef_allocList = (struct _EF_Slot *)newAllocation;
  _ef_allocListSize = newSize;
  slotCount   += slotsPerPage;
  unUsedSlots += slotsPerPage;

#ifndef EF_NO_LEAKDETECTION
  _eff_deallocate( oldAllocation, 0 /*=protectAllocList*/, EFA_INT_DEALLOC, __FILE__, __LINE__ );
#else
  _eff_deallocate( oldAllocation, 0 /*=protectAllocList*/, EFA_INT_DEALLOC);
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
 * Electric Fence wastes lots of memory. I do a best-fit allocator here
 * so that it won't waste even more. It's slow, but thrashing because your
 * working set is too big for a system's RAM is even slower. 
 */

void * _eff_allocate(size_t alignment, size_t userSize, int protectBelow, int fillByte, int protectAllocList, enum _EF_Allocator allocator, enum _EF_FailReturn fail  EF_PARAMLIST_FL)
{
  size_t            count;
  struct _EF_Slot * slot;
  struct _EF_Slot * fullSlot;
  struct _EF_Slot * emptySlots[2];
  ef_number         intAddr, userAddr, protAddr, endAddr;
  size_t            internalSize;

  EF_ASSERT( 0 != _ef_allocList );

  /* check userSize */
  if ( 0 == userSize && !EF_ALLOW_MALLOC_0 )
  {
    #ifndef EF_NO_LEAKDETECTION
      EF_Abort("Allocating 0 bytes, probably a bug: %s(%d)",
               filename, lineno);
    #else
      EF_Abort("Allocating 0 bytes, probably a bug.");
    #endif
  }

  /* check alignment */
  if ( alignment == 0 )
  {
    alignment = 1;
  }
  if ( (int)alignment != ((int)alignment & -(int)alignment) )
  {
    #ifndef EF_NO_LEAKDETECTION
      EF_Abort("Alignment (=%d) is not a power of 2 requested from %s(%d)",
               alignment, filename, lineno);
    #else
      EF_Abort("Alignment (=%d) is not a power of 2", alignment);
    #endif
  }

  /* count and show allocation, if requested */
  numAllocs++;
  if (EF_SHOW_ALLOC)
#ifndef EF_NO_LEAKDETECTION
    EF_Print("\nElectricFence: Allocating %d bytes at %s(%d).", userSize, filename, lineno);
#else
    EF_Print("\nElectricFence: Allocating %d bytes.", userSize);
#endif

  /*
   * If protectBelow is set, all addresses returned by malloc()
   * and company will be page-aligned.
   *
   * The internal size of the buffer is rounded up to the next alignment and page-size
   * boundary, and then we add another page's worth of memory for the dead page.
   */
  /* a bit tricky but no modulo and no if () */
  internalSize = ( (userSize + EF_PAGE_SIZE -1) & ~(EF_PAGE_SIZE -1) )
                 + EF_PAGE_SIZE;
  if ( alignment > EF_PAGE_SIZE )
    internalSize += alignment - EF_PAGE_SIZE;

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
#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
    if (ef_init_done)
#endif
      EF_GET_SEMAPHORE();
#endif
    Page_AllowAccess(_ef_allocList, _ef_allocListSize);
  }

  /*
   * If I'm running out of empty slots, create some more before
   * I don't have enough slots left to make an allocation.
   */
  if ( EFAT_INTERNAL != _eff_allocDesc[allocator].type  &&  unUsedSlots < 7 )
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
  for ( slot = _ef_allocList, count = slotCount ; count > 0; --count, ++slot )
  {
    /*
     * Windows needs special treatment, cause Page_Delete() needs exactly
     * the same memory region as Page_Create()!
     * Thus as a quick hack no memory management is done by EFence.
     */
#if !defined(WIN32)
    if ( EFST_FREE == slot->state  &&  slot->internalSize >= internalSize )
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
    if ( EFST_EMPTY == slot->state )
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

    chunkSize = ( chunkSize + EF_PAGE_SIZE -1 ) & ~( EF_PAGE_SIZE -1 );
#endif
    chunkSizekB = (chunkSize+1023) >>10;


    /* Use up one of the empty slots to make the full slot. */
    if ( !emptySlots[0] )
      EF_Abort("Internal error in allocator: No empty slot 0.\n");
#if !defined(WIN32)
    if ( !emptySlots[1] )
      EF_Abort("Internal error in allocator: No empty slot 1.\n");
#endif

    fullSlot      = emptySlots[0];
    emptySlots[0] = emptySlots[1];

    /* reduce protected memory when we would exceed EF_MAX_ALLOC */
    if ( EF_MAX_ALLOC > 0  &&  sumAllocatedMem + chunkSizekB > EF_MAX_ALLOC )
      reduceProtectedMemory( chunkSizekB );

    fullSlot->internalAddress = Page_Create( chunkSize, 0/*= exitonfail*/ );
    if ( 0 == fullSlot->internalAddress  &&  0L != EF_PROTECT_FREE )
    {
      /* reduce as much protected memory as we need - or at least try so */
      reduceProtectedMemory( (chunkSize+1023) >>10 );
      /* simply try again */
      fullSlot->internalAddress = Page_Create( chunkSize, EF_MALLOC_FAILEXIT );
    }
    if ( fullSlot->internalAddress )
    {
      sumAllocatedMem          += ( (chunkSize +1023) >>10 );
      sumTotalAllocatedMem     += ( (chunkSize +1023) >>10 );
      fullSlot->internalSize    = chunkSize;
      fullSlot->state           = EFST_FREE;
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
      intAddr  = (ef_number)fullSlot->internalAddress;
      endAddr  = intAddr + internalSize;
      userAddr = ( intAddr  + internalSize - EF_PAGE_SIZE - userSize )
                & ~(alignment -1); 
      protAddr = ( userAddr + userSize     + EF_PAGE_SIZE -1)
                & ~(EF_PAGE_SIZE -1);

      /* EF_ASSERT(intAddr <= userAddr && intAddr < protAddr ); */

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
      intAddr  = (ef_number)fullSlot->internalAddress;
      endAddr  = intAddr + internalSize;
      userAddr = ( intAddr + EF_PAGE_SIZE + alignment -1)
                & ~(alignment -1);
      protAddr = ( userAddr & ~(EF_PAGE_SIZE -1) ) - EF_PAGE_SIZE;

      /* EF_ASSERT(intAddr < userAddr && intAddr <= protAddr ); */

      /* Set up the "live" page(s). userAddr == protAddr + EF_PAGE_SIZE ! */
      Page_AllowAccess( (char*)userAddr, internalSize - (userAddr - protAddr) );
      /* Set up the "dead" page(s). */
      Page_DenyAccess( (char*)intAddr, userAddr - intAddr );
    }

    /* => userAddress = internalAddress + EF_PAGE_SIZE */
    fullSlot->userAddress = (char*)userAddr;
    fullSlot->protAddress = (char*)protAddr;
    fullSlot->userSize    = userSize;
    fullSlot->state       = EFST_IN_USE;
    fullSlot->allocator   = allocator;
  #ifndef EF_NO_LEAKDETECTION
    fullSlot->fileSource  = EFFS_ALLOCATION;
  #ifdef EF_USE_FRAMENO
    fullSlot->frame       = frameno;
  #endif
    fullSlot->filename    = (char*)filename;
  #ifdef EF_EXPLICIT_INIT
    /* mark allocations from standard libraries
     * before ef_init() is called with lineno = -1
     * to allow special treatment in leak_checking
     */
    fullSlot->lineno      = (ef_init_done) ? lineno : -1;
  #else
    fullSlot->lineno      = lineno;
  #endif
  #endif

    /* initialise no mans land of slot */
    _eff_init_slack( fullSlot );

  } /* end if ( fullSlot->internalSize ) */


  /*
   * Make the pool's internal memory inaccessable, so that the program
   * being debugged can't stomp on it.
   */
  if ( protectAllocList )
  {
    Page_DenyAccess(_ef_allocList, _ef_allocListSize);
#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
    if (ef_init_done)
#endif
      EF_RELEASE_SEMAPHORE();
#endif
  }

  /* Fill the memory if it was specified to do so. */
  if ( ((char*)userAddr) && fillByte != -1 )
    memset( (char*)userAddr, fillByte, userSize);

  return (char*)userAddr;
}



void   _eff_deallocate(void * address, int protectAllocList, enum _EF_Allocator allocator  EF_PARAMLIST_FL)
{
  struct _EF_Slot   * slot;
  long                internalSizekB;

  if ( 0 == _ef_allocList )
    EF_Abort("free() called before first malloc().");

  if ( 0 == address )
    return;

  if ( protectAllocList )
  {
#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
    if (ef_init_done)
#endif
      EF_GET_SEMAPHORE();
#endif
    Page_AllowAccess(_ef_allocList, _ef_allocListSize);
  }

  if ( !(slot = slotForUserAddress(address)) )
  {
    if ( (slot = nearestSlotForUserAddress(address)) )
    {
    #ifndef EF_NO_LEAKDETECTION
      if ( EFFS_ALLOCATION == slot->fileSource )
        EF_Abort("free(%a): address not from EFence or already freed. Address may be corrupted from %a allocated from %s(%d)",
                 address, slot->userAddress, slot->filename, slot->lineno);
      else if ( EFFS_DEALLOCATION == slot->fileSource )
        EF_Abort("free(%a): address not from EFence or already freed. Address may be corrupted from %a deallocated at %s(%d)",
                 address, slot->userAddress, slot->filename, slot->lineno);
      else
    #endif
        EF_Abort("free(%a): address not from EFence or already freed. Address may be corrupted from %a.",
                 address, slot->userAddress);
    }
    else
      EF_Abort("free(%a): address not from EFence or already freed.", address);
  }

  if ( EFST_ALL_PROTECTED == slot->state || EFST_BEGIN_PROTECTED == slot->state )
  {
  #ifndef EF_NO_LEAKDETECTION
    if ( EFFS_ALLOCATION == slot->fileSource )
      EF_Abort("free(%a): memory already freed. allocated from %s(%d)",
               address, slot->filename, slot->lineno);
    else if ( EFFS_DEALLOCATION == slot->fileSource )
      EF_Abort("free(%a): memory already freed at %s(%d)",
               address, slot->filename, slot->lineno);
    else
  #endif
      EF_Abort("free(%a): memory already freed.", address);
  }
  else if ( _eff_allocDesc[slot->allocator].type != _eff_allocDesc[allocator].type )
  {
  #ifndef EF_NO_LEAKDETECTION
    if ( EFFS_ALLOCATION == slot->fileSource )
      EF_Abort("Free mismatch: allocator '%s' used  at %s(%d)\n  but  deallocator '%s' called at %s(%d)!",
               _eff_allocDesc[slot->allocator].name, slot->filename, slot->lineno,
               _eff_allocDesc[allocator].name, filename, lineno );
    else if ( EFFS_DEALLOCATION == slot->fileSource )
      EF_Abort("Free mismatch: allocator '%s' used \nbut deallocator '%s' called at %s(%d)!",
               _eff_allocDesc[slot->allocator].name,
               _eff_allocDesc[allocator].name, filename, lineno );
    else
  #endif
    EF_Abort("Free mismatch: allocator '%s' used  but  deallocator '%s' called!",
             _eff_allocDesc[slot->allocator].name, _eff_allocDesc[allocator].name );
  }

  /* count and show deallocation, if requested */
  numDeallocs++;
  if (EF_SHOW_ALLOC)
#ifndef EF_NO_LEAKDETECTION
    EF_Print("\nElectricFence: Freeing %d bytes at %s(%d) (Allocated from %s(%d)).", slot->userSize, filename, lineno, slot->filename, slot->lineno);
#else
    EF_Print("\nElectricFence: Freeing %d bytes.", slot->userSize);
#endif

  /* CHECK INTEGRITY OF NO MANS LAND */
  _eff_check_slack( slot );

  if ( EF_FREE_ACCESS )
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

  if ( EF_FREE_WIPES )
    memset(slot->userAddress, EF_FILL, slot->userSize);

  internalSizekB = (slot->internalSize+1023) >>10;

  /* protect memory, that nobody can access it */
  /* Free as much protected memory, that we can protect this one */
    /* is there need? and is there a way to free such much? */
  if ( EF_PROTECT_FREE > 0L
      && sumProtectedMem  + internalSizekB >  EF_PROTECT_FREE
      &&                    internalSizekB <  EF_PROTECT_FREE
      && sumProtectedMem >= internalSizekB
     )
    reduceProtectedMemory( internalSizekB );

  if (   ( EFA_INT_ALLOC != slot->allocator )
      && ( EF_PROTECT_FREE < 0L
          || ( EF_PROTECT_FREE > 0L
              && sumProtectedMem + internalSizekB <= EF_PROTECT_FREE
         )   )
     )
  {
    slot->state = EFST_ALL_PROTECTED;
    Page_DenyAccess(slot->internalAddress, slot->internalSize);
    sumProtectedMem += internalSizekB;

    #ifndef EF_NO_LEAKDETECTION
      if ( lineno )
      {
        slot->fileSource  = EFFS_DEALLOCATION;
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
    slot->state           = EFST_EMPTY;
    slot->allocator       = EFA_INT_ALLOC;
    #ifndef EF_NO_LEAKDETECTION
    slot->fileSource      = EFFS_EMPTY;
    #ifdef EF_USE_FRAMENO
      slot->frame         = 0;
    #endif
      slot->filename      = 0;
      slot->lineno        = 0;
    #endif
  }

  if ( protectAllocList )
  {
    Page_DenyAccess(_ef_allocList, _ef_allocListSize);
#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
    if (ef_init_done)
#endif
      EF_RELEASE_SEMAPHORE();
#endif
  }
}


/*********************************************************/

void * _eff_malloc(size_t size  EF_PARAMLIST_FL)
{
  if ( _ef_allocList == 0 )  _eff_init();  /* This sets EF_ALIGNMENT, EF_PROTECT_BELOW, EF_FILL, ... */
  return _eff_allocate(EF_ALIGNMENT, size, EF_PROTECT_BELOW, EF_FILL, 1 /*=protectAllocList*/, EFA_MALLOC, EF_FAIL_ENV  EF_PARAMS_FL);
}


void * _eff_calloc(size_t nelem, size_t elsize  EF_PARAMLIST_FL)
{
  if ( _ef_allocList == 0 )  _eff_init();  /* This sets EF_ALIGNMENT, EF_PROTECT_BELOW, EF_FILL, ... */
  return _eff_allocate(EF_ALIGNMENT, nelem * elsize, EF_PROTECT_BELOW, 0 /*=fillByte*/, 1 /*=protectAllocList*/, EFA_CALLOC, EF_FAIL_ENV  EF_PARAMS_FL);
}


void   _eff_free(void * baseAdr  EF_PARAMLIST_FL)
{
  if ( _ef_allocList == 0 )  _eff_init();  /* This sets EF_ALIGNMENT, EF_PROTECT_BELOW, EF_FILL, ... */
  _eff_deallocate(baseAdr, 1 /*=protectAllocList*/, EFA_FREE  EF_PARAMS_FL);
}


void * _eff_memalign(size_t alignment, size_t size  EF_PARAMLIST_FL)
{
  if ( _ef_allocList == 0 )  _eff_init();  /* This sets EF_ALIGNMENT, EF_PROTECT_BELOW, EF_FILL, ... */
  return _eff_allocate(alignment, size, EF_PROTECT_BELOW, EF_FILL, 1 /*=protectAllocList*/, EFA_MEMALIGN, EF_FAIL_ENV  EF_PARAMS_FL);
}


void * _eff_realloc(void * oldBuffer, size_t newSize  EF_PARAMLIST_FL)
{
  void * ptr;
  if ( _ef_allocList == 0 )  _eff_init();  /* This sets EF_ALIGNMENT, EF_PROTECT_BELOW, EF_FILL, ... */
#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
  if (ef_init_done)
#endif
    EF_GET_SEMAPHORE();
#endif
  Page_AllowAccess(_ef_allocList, _ef_allocListSize);

  ptr = _eff_allocate(EF_ALIGNMENT, newSize, EF_PROTECT_BELOW, -1 /*=fillByte*/, 0 /*=protectAllocList*/, EFA_REALLOC, EF_FAIL_ENV  EF_PARAMS_FL);

  if ( ptr && oldBuffer )
  {
    struct _EF_Slot * slot = slotForUserAddress(oldBuffer);

    if ( slot == 0 )
      EF_Abort("realloc(%a, %d): address not from malloc().", oldBuffer, newSize);

    if ( newSize > slot->userSize )
    {
      memcpy( ptr, oldBuffer, slot->userSize );
      memset( (char*)ptr + slot->userSize, 0, newSize - slot->userSize );
    }
    else if ( newSize > 0 )
      memcpy(ptr, oldBuffer, newSize);

    _eff_deallocate(oldBuffer, 0 /*=protectAllocList*/, EFA_REALLOC  EF_PARAMS_FL);
  }

  Page_DenyAccess(_ef_allocList, _ef_allocListSize);
#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
  if (ef_init_done)
#endif
    EF_RELEASE_SEMAPHORE();
#endif
  return ptr;
}


void * _eff_valloc(size_t size  EF_PARAMLIST_FL)
{
  if ( _ef_allocList == 0 )  _eff_init();  /* This sets EF_ALIGNMENT, EF_PROTECT_BELOW, EF_FILL, ... */
  return _eff_allocate(EF_PAGE_SIZE, size, EF_PROTECT_BELOW, EF_FILL, 1 /*=protectAllocList*/, EFA_VALLOC, EF_FAIL_ENV  EF_PARAMS_FL);
}


char * _eff_strdup(const char * str  EF_PARAMLIST_FL)
{
  size_t size;
  char * dup;
  unsigned i;

  if ( _ef_allocList == 0 )  _eff_init();  /* This sets EF_ALIGNMENT, EF_PROTECT_BELOW, EF_FILL, ... */

  size = 0;
  while (str[size]) ++size;

  dup = _eff_allocate(EF_PAGE_SIZE, size +1, EF_PROTECT_BELOW, -1 /*=fillByte*/, 1 /*=protectAllocList*/, EFA_STRDUP, EF_FAIL_ENV  EF_PARAMS_FL);

  if (dup)                    /* if successful */
    for (i=0; i<=size; ++i)   /* copy string */
      dup[i] = str[i];

  return dup;
}


void * _eff_memcpy(void *dest, const void *src, size_t size  EF_PARAMLIST_FL)
{
  char       * d = (char *)dest;
  const char * s = (const char *)src;
  unsigned i;

  if ( (s < d  &&  d < s + size) || (d < s  &&  s < d + size) )
    EF_Abort("memcpy(%a, %a, %d): memory regions overlap.", dest, src, size);

  for (i=0; i<size; ++i)
    d[i] = s[i];

  return dest;
}


char * _eff_strcpy(char *dest, const char *src  EF_PARAMLIST_FL)
{
  unsigned i;
  size_t size = strlen(src) +1;

  if ( src < dest  &&  dest < src + size )
    EF_Abort("strcpy(%a, %a): memory regions overlap.", dest, src);

  for (i=0; i<size; ++i)
    dest[i] = src[i];

  return dest;
}


char * _eff_strncpy(char *dest, const char *src, size_t size  EF_PARAMLIST_FL)
{
  size_t srcsize;
  unsigned i;

  if ( size > 0  &&  src < dest  &&  dest < src + size )
    EF_Abort("strncpy(%a, %a, %d): memory regions overlap.", dest, src, size);

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


char * _eff_strcat(char *dest, const char *src  EF_PARAMLIST_FL)
{
  unsigned i;
  size_t destlen = strlen(dest);
  size_t srcsize = strlen(src)  +1;

  if ( src < dest +destlen  &&  dest + destlen < src + srcsize )
    EF_Abort("strcat(%a, %a): memory regions overlap.", dest, src);

  for (i=0; i<srcsize; ++i)
    dest[destlen+i] = src[i];

  return dest;
}


char * _eff_strncat(char *dest, const char *src, size_t size  EF_PARAMLIST_FL)
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
    EF_Abort("strncat(%a, %a, %d): memory regions overlap.", dest, src, size);

  /* copy up to size characters from src to dest */
  for (i=0; i<srclen; ++i)
    dest[destlen+i] = src[i];
  /* append single '\0' character */
  dest[destlen+srclen] = 0;

  return dest;
}


/*********************************************************/


#ifndef EF_NO_GLOBAL_MALLOC_FREE

/*
 * define global functions for malloc(), free(), ..
 */

void * malloc(size_t size)
{
  return _eff_malloc(size  EF_PARAMS_UK);
}


void * calloc(size_t nelem, size_t elsize)
{
  return _eff_calloc(nelem, elsize  EF_PARAMS_UK);
}


void   free(void * address)
{
  _eff_free(address  EF_PARAMS_UK);
}


void * memalign(size_t alignment, size_t size)
{
  return _eff_memalign(alignment, size  EF_PARAMS_UK);
}


void * realloc(void * oldBuffer, size_t newSize)
{
  return _eff_realloc(oldBuffer, newSize  EF_PARAMS_UK);
}


void * valloc(size_t size)
{
  return _eff_valloc(size  EF_PARAMS_UK);
}


char * strdup(const char * str)
{
  return _eff_strdup(str  EF_PARAMS_UK);
}


void * memcpy(void *dest, const void *src, size_t size)
{
  return _eff_memcpy(dest, src, size  EF_PARAMS_UK);
}


char * strcpy(char *dest, const char *src)
{
  return _eff_strcpy(dest, src  EF_PARAMS_UK);
}


char * strncpy(char *dest, const char *src, size_t size)
{
  return _eff_strncpy(dest, src, size  EF_PARAMS_UK);
}


char * strcat(char *dest, const char *src)
{
  return _eff_strcat(dest, src  EF_PARAMS_UK);
}


char * strncat(char *dest, const char *src, size_t size)
{
  return _eff_strncat(dest, src, size  EF_PARAMS_UK);
}


#endif /* EF_NO_GLOBAL_MALLOC_FREE */





#ifndef EF_NO_LEAKDETECTION

/* *********************************************************
 *
 *  void  EF_newFrame(void);
 *
 ***********************************************************/

void  EF_newFrame(void)
{
#ifdef EF_USE_FRAMENO
  ++frameno;
#endif
}


/* *********************************************************
 *
 *  void  EF_delFrame(void);
 *
 ***********************************************************/

void  EF_delFrame(void)
{
#ifdef EF_USE_FRAMENO
  if (-1 != frameno)
  {
#endif
    struct _EF_Slot * slot      = _ef_allocList;
    size_t            count     = slotCount;
    int               nonFreed  = 0;

#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
    if (ef_init_done)
#endif
      EF_GET_SEMAPHORE();
#endif
    Page_AllowAccess(_ef_allocList, _ef_allocListSize);

    for ( ; count > 0; --count, ++slot )
    {
      if (    EFST_IN_USE == slot->state
        #ifdef EF_USE_FRAMENO
           && frameno == slot->frame
        #endif
           && EFA_INT_ALLOC != slot->allocator
        #ifdef EF_EXPLICIT_INIT
           && -1 != slot->lineno
        #endif
         )
      {
        EF_Print("\nElectricFence: ptr=0x%a size=%d alloced from %s(%d) not freed",
          slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        ++nonFreed;
      }
    }
    if (nonFreed)
      EF_Abort("EF_delFrame(): Found non free'd pointers.\n");

    Page_DenyAccess(_ef_allocList, _ef_allocListSize);
#ifndef EF_NO_THREAD_SAFETY
#ifdef EF_EXPLICIT_INIT
    if (ef_init_done)
#endif
      EF_RELEASE_SEMAPHORE();
#endif

  #ifdef EF_USE_FRAMENO
    --frameno;
  }
  #endif
  if (EF_SHOW_ALLOC)
    EF_Print("\nElectricFence: EF_delFrame(): Processed %d allocations and %d deallocations in total.\n", numAllocs, numDeallocs);
}

#endif /* end ifndef EF_NO_LEAKDETECTION */

#endif /* ifndef EF_NO_EFENCE */

