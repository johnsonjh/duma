
/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
 * Copyright (C) 2002-2004 Hayati Ayguen <hayati.ayguen@epost.de>, Procitec GmbH
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
"Electric Fence 2.4.10\n"
"Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>\n"
"Copyright (C) 2002-2004 Hayati Ayguen <hayati.ayguen@epost.de>, Procitec GmbH\n";


static const char unknown_file[] =
  "UNKNOWN (use #include \"efence.h\")";


/*
 * MEMORY_CREATION_SIZE is the amount of memory to get from the operating
 * system at one time. We'll break that memory down into smaller pieces for
 * malloc buffers. One megabyte is probably a good value.
 */
#define      MEMORY_CREATION_SIZE  1024 * 1024


#define ALLOCATED_MASK    ( EFST_ALLOC_MALLOC + EFST_ALLOC_NEW_ELEM + EFST_ALLOC_NEW_ARRAY )

/*
 * Struct Slot contains all of the information about a malloc buffer except
 * for the contents of its memory.
 */
struct _EF_Slot
{
#ifndef EF_NO_LEAKDETECTION
  int                   frame;
  char                * filename;   /* filename of allocation */
  int                   lineno;     /* linenumber of allocation */
#endif
  void                * userAddress;
  void                * internalAddress;
  size_t                userSize;
  size_t                internalSize;
  enum _EF_Slot_State   mode;
};


#ifndef EF_NO_LEAKDETECTION
static int    frameno = 0;
#endif


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
 * EF_PROTECT_FREE is used to control the disposition of memory that is
 * released using free(). It is all-caps so that its name
 * matches the name of the environment variable that is used to set it.
 * If its value is greater non-zero, memory released by free is made
 * inaccessable and never allocated again. Any software that touches free
 * memory will then get a segmentation fault. If its value is zero, freed
 * memory will be available for reallocation, but will still be inaccessable
 * until it is reallocated.
 */
static long   EF_PROTECT_FREE = -1L;

/*
 * EF_MAX_ALLOC is used to control the maximum memory print of the program
 * in total: When the sum of allocated and protected memory would exceed
 * this limit, the protected memory is freed/deleted
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
 * EF_FILL is set to 0-255 if Electric Fence should fill all new allocated
 * memory with the specified value. Set to -1 when Electric Fence should not
 * initialise allocated memory.
 * default is set to initialise with 255, cause many programs rely on
 * initialisation to 0!
 */
static int    EF_FILL = 255;

/*
 * allocationList points to the array of slot structures used to manage the
 * malloc arena.
 */
static struct _EF_Slot * allocationList = 0;

/*
 * allocationListSize is the size of the allocation list. This will always
 * be a multiple of the page size.
 */
static size_t allocationListSize = 0;

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
 * internalUse is set when allocating and freeing the allocator-internal
 * data structures.
 */
static int    internalUse = 0;

/*
 * noAllocationListProtection is set to tell malloc() and free() not to
 * manipulate the protection of the allocation list. This is only set in
 * realloc(), which does it to save on slow system calls, and in
 * allocateMoreSlots(), which does it because it changes the allocation list.
 */
static int    noAllocationListProtection = 0;


/*
 * internal variable: sum of allocated -freed +protected memory in kB
 */
static long   sumAllocatedMem = 0;

/*
 * internal variable: sum of protected memory in kB
 */
static long   sumProtectedMem = 0;


void _eff_assert(const char * exprstr, const char * filename, int lineno)
{
  int *pcAddr = 0;
  EF_Print("\nElectric Fence: EF_ASSERT(%s) failed at\n%s(%d)\n", exprstr, filename, lineno );
  /* this is "really" bad, but it works. assert() from assert.h system header
   * stops only the current thread but the program goes on running under MS Visual C++.
   * This way the program definitely halts.
   */
  while (1)
    *pcAddr++ = 0;
}


/*
 * initialize sets up the memory allocation arena and the run-time
 * configuration information.
 */
static void
initialize(void)
{
  size_t            size = MEMORY_CREATION_SIZE;
  size_t            slack;
  char            * string;
  struct _EF_Slot * slot;

  EF_Print(version);

  EF_GET_SEMAPHORE();

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
   * Check if we should be filling new memory with a value.
   */
  if ( (string = getenv("EF_FILL")) != 0)
  {
    EF_FILL = atoi(string);
    if ( -1 != EF_FILL )
      EF_FILL &= 255;
  }

  /*
   * Figure out how many Slot structures to allocate at one time.
   */
  slotCount = slotsPerPage = EF_PAGE_SIZE / sizeof(struct _EF_Slot);
  allocationListSize = EF_PAGE_SIZE;

  if ( allocationListSize > size )
    size = allocationListSize;

  if ( (slack = size % EF_PAGE_SIZE) != 0 )
    size += EF_PAGE_SIZE - slack;

  /*
   * Allocate memory, and break it up into two malloc buffers. The
   * first buffer will be used for Slot structures, the second will
   * be marked free.
   */
  slot = allocationList = (struct _EF_Slot *)Page_Create(size);
  memset((char *)allocationList, 0, allocationListSize);

  slot[0].internalSize    = slot[0].userSize = allocationListSize;
  slot[0].internalAddress = slot[0].userAddress = allocationList;
  slot[0].mode            = EFST_INTERNAL_USE;
  if ( size > allocationListSize )
  {
    slot[1].internalAddress = slot[1].userAddress
                   = ((char *)slot[0].internalAddress) + slot[0].internalSize;
    slot[1].internalSize    = slot[1].userSize
                   =   size - slot[0].internalSize;
    slot[1].mode            = EFST_FREE;
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

  /*
   * Register atexit()
   */
#ifndef EF_NO_LEAKDETECTION
  atexit( EF_delFrame );
#endif

  EF_RELEASE_SEMAPHORE();
}

/*
 * allocateMoreSlots is called when there are only enough slot structures
 * left to support the allocation of a single malloc buffer.
 */
static void
allocateMoreSlots(void)
{
  size_t  newSize = allocationListSize + EF_PAGE_SIZE;
  void *  newAllocation;
  void *  oldAllocation = allocationList;

  Page_AllowAccess(allocationList, allocationListSize);
  noAllocationListProtection = 1;
  internalUse = 1;

  newAllocation = malloc(newSize);
  memcpy(newAllocation, allocationList, allocationListSize);
  memset(&(((char *)newAllocation)[allocationListSize]), 0, EF_PAGE_SIZE);

  allocationList = (struct _EF_Slot *)newAllocation;
  allocationListSize = newSize;
  slotCount += slotsPerPage;
  unUsedSlots += slotsPerPage;

  free(oldAllocation);

  /*
   * Keep access to the allocation list open at this point, because
   * I am returning to memalign(), which needs that access.
    */
  noAllocationListProtection = 0;
  internalUse = 0;
}

/*
 * delete reductionSizekB amount of memory, which has already
 * been freed but got protected
 */
static void
reduceProtectedMemory( long reductionSizekB )
{
  long              alreadyReducekB = 0;
  struct _EF_Slot * slot            = allocationList;
  struct _EF_Slot * endSlot         = allocationList + slotCount;
  size_t            count           = slotCount;

  while (alreadyReducekB < reductionSizekB && slot != endSlot)
  {
    if (EFST_PROTECTED == slot->mode)
    {
      Page_Delete(slot->internalAddress, slot->internalSize);
      alreadyReducekB += (slot->internalSize+1023) >>10;
      slot->mode = EFST_NOT_IN_USE;
    }
    slot++;
  }
  sumProtectedMem -= alreadyReducekB;
  sumAllocatedMem -= alreadyReducekB;
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

/* extern C_LINKAGE */
static void *
memalign(
          size_t              alignment
        , size_t              userSize
        , enum _EF_Slot_State allocation_mode
  #ifndef EF_NO_LEAKDETECTION
        , const char        * filename
        , int                 lineno
  #endif
        )
{
  size_t            count;
  struct _EF_Slot * slot;
  struct _EF_Slot * fullSlot;
  struct _EF_Slot * emptySlots[2];
  char            * address;
  char            * tmpBegAddr;
  char            * tmpEndAddr;
  size_t            internalSize;
  size_t            userSlack;
  size_t            pageSlack;
  size_t            chunkSlack;

  if ( allocationList == 0 )
    initialize();

  EF_GET_SEMAPHORE();

  if ( internalUse )
    allocation_mode = EFST_INTERNAL_USE;

  if ( userSize == 0 && !EF_ALLOW_MALLOC_0 )
    EF_Abort("\nElectric Fence: Allocating 0 bytes, probably a bug.");

  /*
   * If EF_PROTECT_BELOW is set, all addresses returned by malloc()
   * and company will be page-aligned.
    */
  if ( !EF_PROTECT_BELOW && alignment > 1 )
  {
    userSlack = userSize % alignment;
    if ( userSlack )
      userSlack = alignment - userSlack;
  }
  else
    userSlack = 0;

  /*
   * The internal size of the buffer is rounded up to the next page-size
   * boudary, and then we add another page's worth of memory for the
   * dead page.
   */
  internalSize = userSize + userSlack + EF_PAGE_SIZE; /* userSize + EF_PAGE_SIZE; */
  pageSlack = internalSize % EF_PAGE_SIZE;
  if (pageSlack)
    internalSize += (pageSlack = EF_PAGE_SIZE - pageSlack);

  /*
   * These will hold the addresses of two empty Slot structures, that
   * can be used to hold information for any memory I create, and any
   * memory that I mark free.
   */
  fullSlot      = 0;
  emptySlots[0] = 0;
  emptySlots[1] = 0;

  /*
   * The internal memory used by the allocator is currently
   * inaccessable, so that errant programs won't scrawl on the
   * allocator's arena. I'll un-protect it here so that I can make
   * a new allocation. I'll re-protect it before I return.
    */
  if ( !noAllocationListProtection )
    Page_AllowAccess(allocationList, allocationListSize);

  /*
   * If I'm running out of empty slots, create some more before
   * I don't have enough slots left to make an allocation.
   */
  if ( !internalUse && unUsedSlots < 7 )
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
  for ( slot = allocationList, count = slotCount ; count > 0; count-- )
  {
    if ( slot->mode == EFST_FREE && slot->internalSize >= internalSize )
    {
      if ( !fullSlot || slot->internalSize < fullSlot->internalSize)
      {
        fullSlot = slot;
        if ( slot->internalSize == internalSize && emptySlots[0] )
          break;  /* All done, */
      }
    }
    else if ( slot->mode == EFST_NOT_IN_USE )
    {
      if      ( !emptySlots[0] )    emptySlots[0] = slot;
      else if ( !emptySlots[1] )    emptySlots[1] = slot;
      else if ( fullSlot && fullSlot->internalSize == internalSize )
        break;  /* All done. */
    }
    slot++;
  }
  if ( !emptySlots[0] )
    EF_Abort("\nElectric Fence: Internal error in allocator: No empty slot 0.\n");

  if ( !fullSlot ) {
    /*
     * I get here if I haven't been able to find a free buffer
     * with all of the memory I need. I'll have to create more
     * memory. I'll mark it all as free, and then split it into
     * free and allocated portions later.
     */
    size_t  chunkSize = MEMORY_CREATION_SIZE;
    long    chunkSizekB;

    if ( !emptySlots[1] )
      EF_Abort("\nElectric Fence: Internal error in allocator: No empty slot 1.\n");

    if ( chunkSize < internalSize )
      chunkSize = internalSize;

    if ( (chunkSlack = chunkSize % EF_PAGE_SIZE) != 0 )
      chunkSize += EF_PAGE_SIZE - chunkSlack;

    chunkSizekB = (chunkSize+1023) >>10;

    /* Use up one of the empty slots to make the full slot. */
    fullSlot      = emptySlots[0];
    emptySlots[0] = emptySlots[1];

    /* reduce protected memory when we would exceed EF_MAX_ALLOC */
    if ( EF_MAX_ALLOC > 0  &&  sumAllocatedMem + chunkSizekB > EF_MAX_ALLOC )
      reduceProtectedMemory( chunkSizekB );

    fullSlot->internalAddress = Page_Create( chunkSize );
    if ( 0 == fullSlot->internalAddress  &&  EF_PROTECT_FREE != 0L )
    {
      /* reduce as much protected memory as we need - or at least try so */
      reduceProtectedMemory( (chunkSize+1023) >>10 );
      /* simply try again */
      fullSlot->internalAddress = Page_Create( chunkSize );
    }
    if (fullSlot->internalAddress)
    {
      sumAllocatedMem          += ( (chunkSize+1023) >>10 );
      fullSlot->internalSize    = chunkSize;
      fullSlot->mode            = EFST_FREE;
      unUsedSlots--;
    }
    else
    {
      address                   = (char *)0;
      fullSlot->internalAddress = 0;
      fullSlot->internalSize    = 0;
      fullSlot->userAddress     = 0;
      fullSlot->userSize        = 0;
      fullSlot->mode            = EFST_NOT_IN_USE;
    }
  }

  if ( fullSlot->internalSize )
  {
    /*
     * If I'm allocating memory for the allocator's own data structures,
     * mark it INTERNAL_USE so that no errant software will be able to
     * free it.
     */
    fullSlot->mode = allocation_mode;

    /*
     * If the buffer I've found is larger than I need, split it into
     * an allocated buffer with the exact amount of memory I need, and
     * a free buffer containing the surplus memory.
     */
    if ( fullSlot->internalSize > internalSize ) {
      emptySlots[0]->internalSize    = fullSlot->internalSize - internalSize;
      emptySlots[0]->internalAddress = ((char *)fullSlot->internalAddress) + internalSize;
      emptySlots[0]->mode            = EFST_FREE;
      fullSlot->internalSize         = internalSize;
      unUsedSlots--;
    }

    if ( !EF_PROTECT_BELOW ) {
      /*
       * Arrange the buffer so that it is followed by an inaccessable
       * memory page. A buffer overrun that touches that page will
       * cause a segmentation fault.
       */
      address = (char *)fullSlot->internalAddress;

      /* Set up the "live" page. */
      if ( internalSize - EF_PAGE_SIZE > 0 )
        Page_AllowAccess(fullSlot->internalAddress, internalSize - EF_PAGE_SIZE);
      
      address += internalSize - EF_PAGE_SIZE;

      /* Set up the "dead" page. */
      Page_DenyAccess(address, EF_PAGE_SIZE);

      /* Figure out what address to give the user. */
      address -= (userSize + userSlack);

      /* write some pattern to mark the no mans land */
      /* a- from start to begin */
      tmpBegAddr = (char *)fullSlot->internalAddress;
      tmpEndAddr = tmpBegAddr + pageSlack;
      while (tmpBegAddr < tmpEndAddr)
        *tmpBegAddr++ = (char)EF_FILL;
      /* b- from end to page boundary */
      tmpBegAddr = address + userSize;
      tmpEndAddr = tmpBegAddr + userSlack;
      while (tmpBegAddr < tmpEndAddr)
        *tmpBegAddr++ = (char)EF_FILL;
    }
    else {  /* EF_PROTECT_BELOW != 0 */
      /*
       * Arrange the buffer so that it is preceded by an inaccessable
       * memory page. A buffer underrun that touches that page will
       * cause a segmentation fault.
       */
      address = (char *)fullSlot->internalAddress;

      /* Set up the "dead" page. */
      Page_DenyAccess(address, EF_PAGE_SIZE);

      address += EF_PAGE_SIZE;

      /* Set up the "live" page. */
      if ( internalSize - EF_PAGE_SIZE > 0 )
        Page_AllowAccess(address, internalSize - EF_PAGE_SIZE);

      /* write some pattern to mark the no mans land */
      /* b- from end to page boundary */
      tmpBegAddr = address + userSize;
      tmpEndAddr = tmpBegAddr + pageSlack;
      while (tmpBegAddr < tmpEndAddr)
        *tmpBegAddr++ = (char)EF_FILL;
    }

    fullSlot->userAddress = address;
    fullSlot->userSize    = userSize;
  #ifndef EF_NO_LEAKDETECTION
    fullSlot->frame       = frameno;
    fullSlot->filename    = (char*)filename;
    fullSlot->lineno      = lineno;
  #endif

  } /* end if ( fullSlot->internalSize ) */


  /*
   * Make the pool's internal memory inaccessable, so that the program
   * being debugged can't stomp on it.
   */
  if ( !internalUse )
    Page_DenyAccess(allocationList, allocationListSize);

  EF_RELEASE_SEMAPHORE();

  /* Fill the memory if it was specified to do so. */
  if ( address && EF_FILL != -1 )
    memset( address, EF_FILL, userSize);

  return address;
}

/*
 * Find the slot structure for a user address.
 */
static struct _EF_Slot *
slotForUserAddress(void * address)
{
  struct _EF_Slot * slot  = allocationList;
  size_t            count = slotCount;

  for ( ; count > 0; count-- ) {
    if ( slot->userAddress == address )
      return slot;
    slot++;
  }

  return 0;
}

/*
 * Find the slot structure for an internal address.
 */
static struct _EF_Slot *
slotForInternalAddrNextTo(void * address)
{
  struct _EF_Slot * slot  = allocationList;
  size_t            count = slotCount;

  for ( ; count > 0; count-- ) {
    if ( slot->internalAddress == address )
      return slot;
    slot++;
  }
  return 0;
}


/*
 * Given the internal address of a buffer, find the buffer immediately
 * before that buffer in the address space. This is used by free() to
 * coalesce two free buffers into one.
 */
static struct _EF_Slot *
slotForInternalAddrPrevTo(void * address)
{
  struct _EF_Slot * slot  = allocationList;
  size_t            count = slotCount;

  for ( ; count > 0; count-- ) {
    if ( ((char *)slot->internalAddress) + slot->internalSize == address )
      return slot;
    slot++;
  }
  return 0;
}



#ifndef EF_NO_LEAKDETECTION
void   free(void * address) { _eff_free(address, EFST_ALLOC_MALLOC); }

void   _eff_free(void * address, enum _EF_Slot_State mode)
{
#else
void   free(void * address)
{
  enum _EF_Slot_State mode = EFST_ALLOC_MALLOC;
#endif
  struct _EF_Slot   * slot, * prevSlot, * nextSlot;
  char              * tmpBegAddr, * tmpEndAddr;
  size_t              userSlack, pageSlack;
  long                internalSizekB;

  if ( address == 0 )
    return;

  if ( allocationList == 0 )
    EF_Abort("\nElectric Fence: free() called before first malloc().");

  EF_GET_SEMAPHORE();

  if ( !noAllocationListProtection )
    Page_AllowAccess(allocationList, allocationListSize);

  slot = slotForUserAddress(address);

  if ( !slot )
    EF_Abort("\nElectric Fence: free(%a): address not from malloc().", address);

  if ( 0 == (slot->mode & ALLOCATED_MASK) )
  {
    if ( internalUse && slot->mode == EFST_INTERNAL_USE )
      /* Do nothing. */;
    else
      EF_Abort("\nElectric Fence: free(%a): freeing free memory.", address);
  }
  else
  {
    if (slot->mode != mode)
      EF_Abort("\nFree mismatch: malloc/calloc/realloc/free, new/delete, new[]/delete[].");
  }

  /* CHECK INTEGRITY OF NO MANS LAND */

  /* calculate userSlack */
  if ( !EF_PROTECT_BELOW && EF_ALIGNMENT > 1 ) {
    userSlack = slot->userSize % EF_ALIGNMENT;
    if ( userSlack )
      userSlack = EF_ALIGNMENT - userSlack;
  }
  else
    userSlack = 0;

  /* calculate pageSlack */
  pageSlack = slot->internalSize - slot->userSize - userSlack - EF_PAGE_SIZE;

  /* the check itself */
  if ( !EF_PROTECT_BELOW)
  {
    tmpBegAddr = (char*)slot->internalAddress;
    tmpEndAddr = tmpBegAddr + pageSlack;
    while (tmpBegAddr < tmpEndAddr)
      if (*tmpBegAddr++ != (char)EF_FILL)
        #ifndef EF_NO_LEAKDETECTION
          EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land, size=%d alloced from %s(%d)",
            slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        #else
          EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land", slot->userAddress);
        #endif

    tmpBegAddr = (char*)slot->userAddress + slot->userSize;
    tmpEndAddr = tmpBegAddr + userSlack;
    while (tmpBegAddr < tmpEndAddr)
      if (*tmpBegAddr++ != (char)EF_FILL)
        #ifndef EF_NO_LEAKDETECTION
          EF_Abort("\nfree() detected overwrite of no mans land: ptr=%a, size=%d\nalloced from %s(%d)",
            slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        #else
          EF_Abort("\nfree() detected overwrite of no mans land: ptr=%a", slot->userAddress);
        #endif
  }
  else
  {
    tmpBegAddr = (char*)slot->userAddress + slot->userSize;
    tmpEndAddr = tmpBegAddr + pageSlack;
    while (tmpBegAddr < tmpEndAddr)
      if (*tmpBegAddr++ != (char)EF_FILL)
        #ifndef EF_NO_LEAKDETECTION
          EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land, size=%d alloced from %s(%d)",
            slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        #else
          EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land", slot->userAddress);
        #endif
  }


  /*
   * Free memory is _always_ set to deny access. When EF_PROTECT_FREE
   * is true, free memory is never reallocated, so it remains access
   * denied for the life of the process. When EF_PROTECT_FREE is false, 
   * the memory may be re-allocated, at which time access to it will be
   * allowed again.
   *
   * Some operating systems allow munmap() with single-page resolution,
   * and allow you to un-map portions of a region, rather than the
   * entire region that was mapped with mmap(). On those operating
   * systems, we can release protected free pages with Page_Delete(),
   * in the hope that the swap space attached to those pages will be
   * released as well.
   */
  if ( EF_PROTECT_FREE > 0L )
  {
    /* Free as much protected memory, that we can protect this one */
    internalSizekB = (slot->internalSize+1023) >>10;

    /* is there need? and is there a way to free such much? */
    if (   sumProtectedMem  + internalSizekB >  EF_PROTECT_FREE
        &&                    internalSizekB <  EF_PROTECT_FREE
        && sumProtectedMem >= internalSizekB
       )
      reduceProtectedMemory( internalSizekB );
  }

  if ( EF_PROTECT_FREE > 0L  &&  sumProtectedMem + internalSizekB <= EF_PROTECT_FREE )
  {
    slot->mode = EFST_PROTECTED;
    Page_DenyAccess(slot->internalAddress, slot->internalSize);
    sumProtectedMem += internalSizekB;

    prevSlot = slotForInternalAddrPrevTo( slot->internalAddress );
    if ( prevSlot && prevSlot->mode == slot->mode ) {
      /* Coalesce previous slot with this one. */
      prevSlot->internalSize += slot->internalSize;
      slot->internalAddress   = slot->userAddress   = 0;
      slot->internalSize      = slot->userSize      = 0;
      slot->mode              = EFST_NOT_IN_USE;
      slot                    = prevSlot;
      unUsedSlots++;
    }
    else
    {
      nextSlot = slotForInternalAddrNextTo(
                      ((char *)slot->internalAddress) +slot->internalSize  );
      if ( nextSlot && nextSlot->mode == slot->mode ) {
        /* Coalesce next slot with this one. */
        slot->internalSize        += nextSlot->internalSize;
        nextSlot->internalAddress  = nextSlot->userAddress  = 0;
        nextSlot->internalSize     = nextSlot->userSize     = 0;
        nextSlot->mode             = EFST_NOT_IN_USE;
        unUsedSlots++;
      }
    }

  }
  else
  {
    slot->mode = EFST_NOT_IN_USE;
    Page_Delete(slot->internalAddress, slot->internalSize);
    sumAllocatedMem -= ( (slot->internalSize+1023) >>10 );
  }

  slot->userAddress = slot->internalAddress;
  slot->userSize    = slot->internalSize;

  if ( !noAllocationListProtection )
    Page_DenyAccess(allocationList, allocationListSize);

  EF_RELEASE_SEMAPHORE();
}


#ifndef EF_NO_LEAKDETECTION
void * realloc(void * oldBuffer, size_t newSize) { return _eff_realloc(oldBuffer, newSize, EFST_ALLOC_MALLOC, unknown_file, 0); }

void * _eff_realloc(void * oldBuffer, size_t newSize, enum _EF_Slot_State mode, const char * filename, int lineno)
{
#else
void * realloc(void * oldBuffer, size_t newSize)
{
  enum _EF_Slot_State mode = EFST_ALLOC_MALLOC;
#endif
  void *  newBuffer;

  if ( allocationList == 0 )
    initialize();  /* This sets EF_ALIGNMENT */

  EF_GET_SEMAPHORE();

  newBuffer = malloc(newSize);

  if ( oldBuffer )
  {
    struct _EF_Slot * slot;
    size_t            size;

    Page_AllowAccess(allocationList, allocationListSize);
    noAllocationListProtection = 1;
    
    slot = slotForUserAddress(oldBuffer);

    if ( slot == 0 )
      EF_Abort("\nElectric Fence: realloc(%a, %d): address not from malloc().", oldBuffer, newSize);

    if ( newSize < (size = slot->userSize) )
      size = newSize;

    if ( size > 0 )
      memcpy(newBuffer, oldBuffer, size);

    free(oldBuffer);
    noAllocationListProtection = 0;
    Page_DenyAccess(allocationList, allocationListSize);

    if ( size < newSize )
      memset(&(((char *)newBuffer)[size]), 0, newSize - size);
    
    /* Internal memory was re-protected in free() */
  }

  EF_RELEASE_SEMAPHORE();

  return newBuffer;
}


#ifndef EF_NO_LEAKDETECTION
void * malloc(size_t size) { return _eff_malloc(size, EFST_ALLOC_MALLOC, unknown_file, 0); }

void * _eff_malloc(size_t size, enum _EF_Slot_State mode, const char * filename, int lineno)
{
#else
void * malloc(size_t size)
{
  enum _EF_Slot_State mode = EFST_ALLOC_MALLOC;
#endif

  if ( allocationList == 0 )
    initialize();  /* This sets EF_ALIGNMENT */

#ifndef EF_NO_LEAKDETECTION
  return memalign(EF_ALIGNMENT, size, mode, filename, lineno);
#else
  return memalign(EF_ALIGNMENT, size, mode);
#endif
}


#ifndef EF_NO_LEAKDETECTION
void * calloc(size_t nelem, size_t elsize) { return _eff_calloc(nelem, elsize, EFST_ALLOC_MALLOC, unknown_file, 0); }

void * _eff_calloc(size_t nelem, size_t elsize, enum _EF_Slot_State mode, const char * filename, int lineno)
{
#else
void * calloc(size_t nelem, size_t elsize)
{
  enum _EF_Slot_State mode = EFST_ALLOC_MALLOC;
#endif
  size_t  size = nelem * elsize;
  void *  allocation; /* = malloc(size); */

  if ( allocationList == 0 )
    initialize();  /* This sets EF_ALIGNMENT */

#ifndef EF_NO_LEAKDETECTION
  allocation = memalign(EF_ALIGNMENT, size, mode, filename, lineno);
#else
  allocation = memalign(EF_ALIGNMENT, size, mode);
#endif

  memset(allocation, 0, size);
  return allocation;
}

/*
 * This will catch more bugs if you remove the page alignment, but it
 * will break some software.
 */
void *valloc (size_t size)
{
#ifndef EF_NO_LEAKDETECTION
  return memalign(EF_PAGE_SIZE, size, EFST_ALLOC_MALLOC, unknown_file, 0);
#else
  return memalign(EF_PAGE_SIZE, size, EFST_ALLOC_MALLOC);
#endif
}


#ifdef __hpux
/*
 * HP-UX 8/9.01 strcat reads a word past source when doing unaligned copies!
 * Work around it here. The bug report has been filed with HP.
 */
char *strcat(char *d, const char *s)
{
  strcpy(d+strlen(d), s);
  return d;
}
#endif



#ifndef EF_NO_LEAKDETECTION

/* *********************************************************
 *
 *  void  EF_newFrame(void);
 *
 ***********************************************************/

void  EF_newFrame(void)
{
  ++frameno;
}


/* *********************************************************
 *
 *  void  EF_delFrame(void);
 *
 ***********************************************************/

void  EF_delFrame(void)
{
  if (-1 != frameno)
  {
    struct _EF_Slot * slot      = allocationList;
    size_t            count     = slotCount;
    int               nonFreed  = 0;

    Page_AllowAccess(allocationList, allocationListSize);

    for ( ; count > 0; --count )
    {
      if ( frameno == slot->frame && (slot->mode & ALLOCATED_MASK) )
      {
        EF_Print("\nElectric Fence: ptr=0x%a size=%d alloced from %s(%d) not freed",
          slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        ++nonFreed;
      }
      ++slot;
    }
    if (nonFreed)
      EF_Abort("\nElectric Fence: EF_delFrame(): Found non free'd pointers.\n");

    Page_DenyAccess(allocationList, allocationListSize);

    --frameno;
  }
}

#endif /* end ifndef EF_NO_LEAKDETECTION */

#endif /* ifndef EF_NO_EFENCE */
