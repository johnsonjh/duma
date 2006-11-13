
/* 
 * KDUMA - Kernel Mode Red-Zone memory allocator.
 * Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
 * Copyright (C) 2006 Eric Rachner <eric@rachner.us>
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
 */

/* $Id$ */

static const char  version[] = 
	"KDUMA v0.1 -- Red-Zone Memory Allocator\n"
	"Copyright (c) 2006 Michael Eddington\n"
	"Copyright (c) 2006 Eric Rachner\n";
	
static const char unknown_file[] =
  "UNKNOWN (use #include \"kduma.h\")";

/* Variable: MEMORY_CREATION_SIZE
 *
 * MEMORY_CREATION_SIZE is the amount of memory to get from the operating
 * system at one time. We'll break that memory down into smaller pieces for
 * malloc buffers. One megabyte is probably a good value.
 */
#define      MEMORY_CREATION_SIZE  1024 * 1024

/* Enum: _DUMA_SlotState
 *
 * State of slot values (empty, free, etc)
 */
enum _DUMA_SlotState
{
    DUMAST_EMPTY			/* slot not in use */
  , DUMAST_FREE				/* internal memory reserved, unused by user */
  , DUMAST_IN_USE			/* memory in use by allocator; see following enum AllocType */
  , DUMAST_ALL_PROTECTED	/* memory no more used by allocator; memory is not deallocated but protected */
  , DUMAST_BEGIN_PROTECTED	/* most memory deallocated, but not page covering userAddress:
							 * slot holds userAddress, userSize and allocator.
							 */
};

/* Enum: _DUMA_Slot_FileSource
 *
 * Where did we get file info
 */
enum _DUMA_Slot_FileSource
{
    DUMAFS_EMPTY			/* no filename, lineno */
  , DUMAFS_ALLOCATION		/* filename, lineno from allocation */
  , DUMAFS_DEALLOCATION		/* filename, lineno from deallocation */
};

/* Enum: _DUMA_InitState
 *
 * Whats are initialization start
 */
enum _DUMA_InitState
{
    DUMAIS_UNINITIALIZED = 0x1611	/* not initialized */
  , DUMAIS_IN_CONSTRUCTOR			/* in constructor _duma_init() */
  , DUMAIS_OUT_CONSTRUCTOR			/* construction _duma_init() finished */
  , DUMAIS_IN_INIT					/* in initializer duma_init() */
  , DUMAIS_OUT_INIT					/* initialization duma_init() finished */
};

/*
 * Struct Slot contains all of the information about a malloc buffer except
 * for the contents of its memory.
 */
struct _DUMA_Slot
{
	void			*internalAddress;
	void			*userAddress;
	void			*protAddress;
	size_t			internalSize;
	size_t			userSize;

	/* save (some) space in production */
	unsigned short	state       :16;
	unsigned short	allocator   :8;
	unsigned short	fileSource  :8;

#ifdef DUMA_USE_FRAMENO
	int					frame;
#endif
	char				*filename;   /* filename of allocation */
	int					lineno;     /* linenumber of allocation */
#endif

#ifdef DUMA_EXPLICIT_INIT
	int					slackfill;
#endif
};

enum _DUMA_AllocType
{
    DUMAAT_INTERNAL
  , DUMAAT_MALLOC
  , DUMAAT_NEW_ELEM
  , DUMAAT_NEW_ARRAY
  , DUMAAT_MEMBER_NEW_ELEM
  , DUMAAT_MEMBER_NEW_ARRAY
};

static struct _DUMA_AllocDesc
{
  char					*name;
  enum _DUMA_AllocType	type;
}

/* Variable: DUMA_ALIGNMENT
 *
 * DUMA_ALIGNMENT is a global variable used to control the default alignment
 * of buffers returned by malloc(), calloc(), and realloc(). It is all-caps
 * so that its name matches the name of the environment variable that is used
 * to set it. This gives the programmer one less name to remember.
 */
size_t DUMA_ALIGNMENT = DUMA_MIN_ALIGNMENT;

/* Variable: DUMA_PROTECT_BELOW
 *
 * DUMA_PROTECT_BELOW is used to modify the behavior of the allocator. When
 * its value is non-zero, the allocator will place an inaccessable page
 * immediately _before_ the malloc buffer in the address space, instead
 * of _after_ it. Use this to detect malloc buffer under-runs, rather than
 * over-runs. It won't detect both at the same time, so you should test your
 * software twice, once with this value clear, and once with it set.
 */
int DUMA_PROTECT_BELOW = 0;

/* Variable: DUMA_FILL
 *
 * DUMA_FILL is set to 0-255 if DUMA should fill all new allocated
 * memory with the specified value. Set to -1 when DUMA should not
 * initialise allocated memory.
 * default is set to initialise with 255, cause many programs rely on
 * initialisation to 0!
 */
int DUMA_FILL = 255;

/* Variable: DUMA_SLACKFILL
 *
 * DUMA_SLACKFILL is set to 0-255. The slack / no mans land of all new allocated
 * memory is filled with the specified value.
 * default is set to initialise with 0xAA (=binary 10101010)
 * initialisation to 0!
 */
static int DUMA_SLACKFILL = 0xAA;

/* Variable: DUMA_PROTECT_FREE
 *
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
static long DUMA_PROTECT_FREE = -1L;

/* Variable: DUMA_MAX_ALLOC
 *
 * DUMA_MAX_ALLOC is used to control the maximum memory print of the program
 * in total: When the sum of allocated and protected memory would exceed
 * this value in kB, the protected memory is freed/deleted.
 */
static long DUMA_MAX_ALLOC = -1L;

/* Variable: DUMA_ALLOW_MALLOC_0
 *
 * DUMA_ALLOW_MALLOC_0 is set if DUMA is to allow malloc(0). I
 * trap malloc(0) by default because it is a common source of bugs.
 * But you should know the allocation with size 0 is ANSI conform.
 */
static int DUMA_ALLOW_MALLOC_0 = 1;

/* Variable: DUMA_MALLOC_FAILEXIT
 *
 * DUMA_MALLOC_FAILEXIT controls the behaviour of DUMA when
 * malloc() fails and would return NULL. But most applications don't
 * check the return value for errors ... so
 * default to Exit on Fail
 */
static int DUMA_MALLOC_FAILEXIT = 1;

/* Variable: DUMA_FREE_ACCESS
 *
 * DUMA_FREE_ACCESS is set if DUMA is to write access memory before
 * freeing it. This makes easier using watch expressions in debuggers as the
 * process is interrupted even if the memory is going to be freed.
 */
static int DUMA_FREE_ACCESS = 0;

/* Variable: DUMA_SHOW_ALLOC
 *
 * DUMA_SHOW_ALLOC is set if DUMA is to print all allocations
 * and deallocations to the console. Although this generates a lot
 * of messages, the option can be useful to detect inefficient code
 * containing many allocations / deallocations
 */
static int DUMA_SHOW_ALLOC = 0;

/* Variable: _duma_allocList
 *
 * _DUMA_allocList points to the array of slot structures used to manage the
 * malloc arena.
 */
struct _DUMA_Slot * _duma_allocList = 0;

/* Variable: _duma_allocListSize
 *
 * _duma_allocListSize is the size of the allocation list. This will always
 * be a multiple of the page size.
 */
static size_t _duma_allocListSize = 0;

/* Variable: slotCount
 *
 * slotCount is the number of Slot structures in allocationList.
 */
static size_t slotCount = 0;

/* Variable: unUsedSlots
 *
 * unUsedSlots is the number of Slot structures that are currently available
 * to represent new malloc buffers. When this number gets too low, we will
 * create new slots.
 */
static size_t unUsedSlots = 0;

/* Variable: slotsPerPage
 *
 * slotsPerPage is the number of slot structures that fit in a virtual
 * memory page.
 */
static size_t slotsPerPage = 0;

/* Variable: sumAllocatedMem
 *
 * internal variable: sum of allocated -freed +protected memory in kB
 */
static long   sumAllocatedMem = 0;

/* Variable: sumTotalAllocatedMem
 *
 * internal variable: sum of allocated memory in kB
 */
static long   sumTotalAllocatedMem = 0;

/* Variable: sumProtectedMem
 *
 * internal variable: sum of protected memory in kB
 */
static long   sumProtectedMem = 0;

/* Variable: numDeallocs
 *
 * internal variable: number of deallocations processed so far
 */
static long numDeallocs = 0;

/* Variable: numAllocs
 *
 * internal variable: number of allocations processed so far
 */
static long numAllocs = 0;

/* Variable: duma_init_done
 *
 * internal variable: state of initialization
 */
static enum _DUMA_InitState duma_init_state = DUMAIS_UNINITIALIZED;

/* Function: _duma_assert
 *
 * Print message and halt program execution in crazy way.
 */
void _duma_assert(const char * exprstr, const char * filename, int lineno)
{
	// !!!! Needs to be re implemented for KDUMA
	int *pcAddr = 0;
	DUMA_Print("\nDUMA: DUMA_ASSERT(%s) failed at\n%s(%i)\n", exprstr, filename, lineno );

	/* this is "really" bad, but it works. assert() from assert.h system header
	* stops only the current thread but the program goes on running under MS Visual C++.
	* This way the program definitely halts.
	*/
	while (1)
		*pcAddr++ = 0;
}

/* Function: duma_init
 *
 * duma_init sets configuration settings.  Can sometimes cause problems
 * when called from _duma_init.
 *
 * See Also: <_duma_init>
 */
void duma_init(void)
{
	char            * string;
	void            * testAlloc;

	/* avoid double call, when initialization already in progress */
	if ( duma_init_state >= DUMAIS_IN_INIT && duma_init_state <= DUMAIS_OUT_INIT )
		return;
	else
		duma_init_state = DUMAIS_IN_INIT;
	
	DUMA_Print(version);
	DUMA_Print("DUMA: Registration was successful.\n");

	/* initialize semaphoring */
	DUMA_INIT_SEMAPHORE();

#ifndef DUMA_NO_GLOBAL_MALLOC_FREE
	/*
	 * Check whether malloc and free is available
	 */
	testAlloc = kmalloc(123);
	if (numAllocs == 0)
		DUMA_Abort("kmalloc() is not bound to kduma.\nKDUMA Aborting.\n");

	kfree(testAlloc);
	if (numDeallocs == 0)
		DUMA_Abort("kfree() is not bound to kduma.\nKDUMA Aborting.\n");
#endif

	/* initialization finished */
	duma_init_state = DUMAIS_OUT_INIT;
}


/* Function: _duma_init
 *
 * _duma_init sets up the memory allocation arena and the run-time
 * configuration information.  We will call duma_init unless DUMA_EXPLICIT_INIT
 * is defined at compile time.
 *
 * See Also: <duma_init>
 */
void
_duma_init(void)
{
	size_t size = MEMORY_CREATION_SIZE;
	struct _DUMA_Slot * slot;
	int               inRecursion = (duma_init_state >= DUMAIS_IN_CONSTRUCTOR && duma_init_state <= DUMAIS_OUT_INIT);

	/* constuction already done? this should not happen! */
	if (duma_init_state >= DUMAIS_OUT_CONSTRUCTOR && duma_init_state <= DUMAIS_OUT_INIT)
		goto duma_constructor_callinit;
	else
		duma_init_state = DUMAIS_IN_CONSTRUCTOR;

	if ( DUMA_PAGE_SIZE != Page_Size() )
		DUMA_Abort("DUMA_PAGE_SIZE is not correct. Run createconf and save results as duma_config.h");

	if(!inRecursion)
		DUMA_GET_SEMAPHORE();

	/* call of DUMA_GET_SEMAPHORE() may already have done the construction recursively! */
	if ( duma_init_state >= DUMAIS_OUT_CONSTRUCTOR )
		goto duma_constructor_relsem;
	
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
	slot = _duma_allocList = (struct _DUMA_Slot *)Page_Create(size, 0/*=exitonfail*/, 0/*=printerror*/);
	if ( 0 == _duma_allocList  &&  0L != DUMA_PROTECT_FREE )
	{
		int reduce_more;
		do
		{
			/* reduce as much protected memory as we need - or at least try so */
			reduce_more = reduceProtectedMemory( (size+1023) >>10 );
			/* simply try again */
			slot = _duma_allocList = (struct _DUMA_Slot *)Page_Create( size, 0/*=exitonfail*/, 0/*= printerror*/ );
		}
		while ( reduce_more && 0 == _duma_allocList );

		if ( 0 == _duma_allocList )
			slot = _duma_allocList = (struct _DUMA_Slot *)Page_Create( size, 1/*=exitonfail*/, 1/*= printerror*/ );
	}
	
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

	/* construction done */
	if ( duma_init_state < DUMAIS_OUT_CONSTRUCTOR )
		duma_init_state = DUMAIS_OUT_CONSTRUCTOR;
	
	/***********************/
duma_constructor_relsem:
	if ( !inRecursion )
		DUMA_RELEASE_SEMAPHORE();

	/*************************/
duma_constructor_callinit:
	if ( duma_init_state < DUMAIS_OUT_INIT )
		duma_init();
}


/* Function: allocateMoreSlots
 *
 * allocateMoreSlots is called when there are only enough slot structures
 * left to support the allocation of a single malloc buffer.
 *
 * See Also: <_duma_allocate>
 */
static void allocateMoreSlots(void)
{
	size_t  newSize = _duma_allocListSize + DUMA_PAGE_SIZE;
	void *  newAllocation;
	void *  oldAllocation = _duma_allocList;
	
	newAllocation = _duma_allocate( 1 /*=alignment*/, newSize, 
		0 /*=protectBelow*/, -1 /*=fillByte*/, 0 /*=protectAllocList*/, 
		EFA_INT_ALLOC, DUMA_FAIL_NULL, __FILE__, __LINE__ );
	
	if ( ! newAllocation )
		return;
	
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

/* Function: _duma_allocate
 * 
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
 * DUMA wastes lots of memory.
 *
 * See Also: <_duma_deallocate>
 */
void * _duma_allocate(size_t alignment, size_t userSize, int protectBelow, int fillByte, int protectAllocList, enum _DUMA_Allocator allocator, enum _DUMA_FailReturn fail  DUMA_PARAMLIST_FL)
{
	size_t            count;
	struct _DUMA_Slot * slot;
	struct _DUMA_Slot * fullSlot;
	struct _DUMA_Slot * emptySlots[2];
	DUMA_ADDR           intAddr, userAddr, protAddr, endAddr;
	size_t              internalSize;
	char				stacktrace[601];
	char*				ptrStacktrace;

	DUMA_ASSERT( 0 != _duma_allocList );

#ifdef WIN32
	// When getting the stack trace memory will be allocated
	// via DUMA.  In situations were additional slots must
	// be allocated we must do this prior to getting a pointer
	// to the new empty slot.  For this reason please leave
	// this code at the top of this function.
	if(!_DUMA_IN_DUMA && duma_init_state && DUMA_OUTPUT_STACKTRACE)
	{
		_DUMA_IN_DUMA = 1;

		printStackTrace(stacktrace, sizeof(stacktrace), DUMA_OUTPUT_STACKTRACE_MAPFILE);
		internalSize = strlen(stacktrace) * sizeof(char) + 1;
		ptrStacktrace = (char*) LocalAlloc(NULL, internalSize);
		strcpy(ptrStacktrace, stacktrace);
		memset(stacktrace, 0, 600);

		_DUMA_IN_DUMA = 0;
	}
#endif


	/* initialize return value */
	userAddr = 0;

	/* check userSize */
	if ( 0 == userSize )
	{
		if ( !DUMA_ALLOW_MALLOC_0 )
		{
			#ifndef DUMA_NO_LEAKDETECTION
				DUMA_Abort("Allocating 0 bytes, probably a bug: %s(%i)", filename, lineno);
			#else
				DUMA_Abort("Allocating 0 bytes, probably a bug.");
			#endif
		}
		else
			return (void*)userAddr;
	}

	/* check alignment */
	if ( ! alignment )
	{
		DUMA_SIZE a = (DUMA_SIZE)DUMA_ALIGNMENT;
		DUMA_SIZE s = (DUMA_SIZE)userSize;

		if ( s < a )
		{
			/* to next lower power of 2 */
			for (a = s; a & (a-1); a &= a-1)  ;
		}

		alignment = (size_t)a; /* this is new alignment */
	}
	
	if ( (int)alignment != ((int)alignment & -(int)alignment) )
	{
		#ifndef DUMA_NO_LEAKDETECTION
			DUMA_Abort("Alignment (=%d) is not a power of 2 requested from %s(%i)", (DUMA_SIZE)alignment, filename, lineno);
		#else
			DUMA_Abort("Alignment (=%d) is not a power of 2", (DUMA_SIZE)alignment);
		#endif
	}

	/* count and show allocation, if requested */
	numAllocs++;
	if (DUMA_SHOW_ALLOC)
	{
		#ifndef DUMA_NO_LEAKDETECTION
			DUMA_Print("\nDUMA: Allocating %d bytes at %s(%i).", (DUMA_SIZE)userSize, filename, lineno);
		#else
			DUMA_Print("\nDUMA: Allocating %d bytes.", (DUMA_SIZE)userSize);
		#endif
	}

	/*
	 * If protectBelow is set, all addresses returned by malloc()
	 * and company will be page-aligned.
	 *
	 * The internal size of the buffer is rounded up to the next alignment and page-size
	 * boundary, and then we add another page's worth of memory for the dead page.
	 */
	/* a bit tricky but no modulo and no if () */
	internalSize = ( (userSize + DUMA_PAGE_SIZE -1) & ~(DUMA_PAGE_SIZE -1) ) + DUMA_PAGE_SIZE;
	
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
		IF__DUMA_INIT_DONE
			DUMA_GET_SEMAPHORE();

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
				if(!emptySlots[0])
					emptySlots[0] = slot;
				else if ( !emptySlots[1] )
					emptySlots[1] = slot;
	
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
		size_t    chunkSizekB;

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

		fullSlot->internalAddress = Page_Create( chunkSize, 0/*= exitonfail*/, 0/*= printerror*/ );
		
		if ( 0 == fullSlot->internalAddress  &&  0L != DUMA_PROTECT_FREE )
		{
			int reduce_more;
			do
			{
				/* reduce as much protected memory as we need - or at least try so */
				reduce_more = reduceProtectedMemory( (chunkSize+1023) >>10 );
				/* simply try again */
				fullSlot->internalAddress = Page_Create( chunkSize, 0/*= exitonfail*/, 0/*= printerror*/ );
			}
			while ( reduce_more && 0 == fullSlot->internalAddress );

			if ( 0 == fullSlot->internalAddress  &&  DUMA_FAIL_ENV == fail )
				fullSlot->internalAddress = Page_Create( chunkSize, DUMA_MALLOC_FAILEXIT, 1/*= printerror*/ );
		}

		if ( fullSlot->internalAddress )
		{
			sumAllocatedMem          += ( (chunkSize +1023) >>10 );
			sumTotalAllocatedMem     += ( (chunkSize +1023) >>10 );
			fullSlot->internalSize    = chunkSize;
			fullSlot->state           = DUMAST_FREE;
			--unUsedSlots;
		}
	} /* end if ( !fullSlot ) */

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
			emptySlots[0]->internalAddress = (char *)emptySlots[0]->internalAddress + internalSize;
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
			intAddr  = (DUMA_ADDR)fullSlot->internalAddress;
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
			intAddr  = (DUMA_ADDR)fullSlot->internalAddress;
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
				* before duma_init() is finished with lineno = -1
				* to allow special treatment in leak_checking
				*/
				fullSlot->lineno      = (DUMAIS_OUT_INIT == duma_init_state) ? lineno : -1;
			#else
				fullSlot->lineno      = lineno;
			#endif
		#endif

		/* initialise no mans land of slot */
		_duma_init_slack( fullSlot );

#ifdef WIN32
		if(!_DUMA_IN_DUMA && duma_init_state && DUMA_OUTPUT_STACKTRACE)
		{
			_DUMA_IN_DUMA = 1;

			/* Get stacktrace */
			if(fullSlot->stacktrace)
				LocalFree(fullSlot->stacktrace);

			fullSlot->stacktrace = ptrStacktrace;

			_DUMA_IN_DUMA = 0;
		}
		else
			fullSlot->stacktrace = 0;
#endif

	} /* end if ( fullSlot->internalSize ) */

	/*
	 * Make the pool's internal memory inaccessable, so that the program
	 * being debugged can't stomp on it.
	 */
	if ( protectAllocList )
	{
		Page_DenyAccess(_duma_allocList, _duma_allocListSize);

		IF__DUMA_INIT_DONE
			DUMA_RELEASE_SEMAPHORE();
	}

	/* Fill the memory if it was specified to do so. */
	if ( ((char*)userAddr) && fillByte != -1 )
		memset( (char*)userAddr, fillByte, userSize);

	return (char*)userAddr;
}


/* Function: _duma_deallocate
 * 
 * Deallocate allocated memory after running some checks, then open
 * slot for use.  Uses Page_Delete to free the underlying memory.
 *
 * See Also: <Page_Delete> <_duma_allocate>
 */
void _duma_deallocate(void * address, int protectAllocList, enum _DUMA_Allocator allocator  DUMA_PARAMLIST_FL)
{
	struct _DUMA_Slot   * slot;
	long                internalSizekB;

	if ( 0 == _duma_allocList )
	{
#ifdef DUMA_DETOURS
		// Odd things happen with detours sometimes...
		DUMA_Print("DUMA_Warning: free() called before first malloc().");
		return;
#else
		DUMA_Abort("free() called before first malloc().");
#endif
	}

	if ( 0 == address )
		return;

	if ( protectAllocList )
	{
		IF__DUMA_INIT_DONE
			DUMA_GET_SEMAPHORE();

		Page_AllowAccess(_duma_allocList, _duma_allocListSize);
	}

	if ( !(slot = slotForUserAddress(address)) )
	{
		if ( (slot = nearestSlotForUserAddress(address)) )
		{
#ifndef DUMA_NO_LEAKDETECTION
			if ( DUMAFS_ALLOCATION == slot->fileSource )
				DUMA_Abort("free(%a): address not from DUMA or already freed. Address may be corrupted from %a allocated from %s(%i)",
					(DUMA_ADDR)address, (DUMA_ADDR)slot->userAddress, slot->filename, slot->lineno);
			else if ( DUMAFS_DEALLOCATION == slot->fileSource )
				DUMA_Abort("free(%a): address not from DUMA or already freed. Address may be corrupted from %a deallocated at %s(%i)",
					(DUMA_ADDR)address, (DUMA_ADDR)slot->userAddress, slot->filename, slot->lineno);
			else
#endif
				DUMA_Abort("free(%a): address not from DUMA or already freed. Address may be corrupted from %a.",
					(DUMA_ADDR)address, (DUMA_ADDR)slot->userAddress);
		}
		else
		{
#if DUMA_DETOURS
			// For Detours we need to not dump out, we get one extra free up front for some reason.
			DUMA_Print("DUMA_Warning: free(%a): address not from DUMA or already freed.", (DUMA_ADDR)address);
			return;
#else
			DUMA_Abort("free(%a): address not from DUMA or already freed.", (DUMA_ADDR)address);
#endif
		}
	}

	if ( DUMAST_ALL_PROTECTED == slot->state || DUMAST_BEGIN_PROTECTED == slot->state )
	{
#ifndef DUMA_NO_LEAKDETECTION
		if ( DUMAFS_ALLOCATION == slot->fileSource )
			DUMA_Abort("free(%a): memory already freed. allocated from %s(%i)",
				(DUMA_ADDR)address, slot->filename, slot->lineno);
		else if ( DUMAFS_DEALLOCATION == slot->fileSource )
			DUMA_Abort("free(%a): memory already freed at %s(%i)",
				(DUMA_ADDR)address, slot->filename, slot->lineno);
		else
#endif
			DUMA_Abort("free(%a): memory already freed.", (DUMA_ADDR)address);
	}
	else if ( _duma_allocDesc[slot->allocator].type != _duma_allocDesc[allocator].type )
	{
#ifndef DUMA_NO_LEAKDETECTION
		if ( DUMAFS_ALLOCATION == slot->fileSource )
			DUMA_Abort("Free mismatch: allocator '%s' used  at %s(%i)\n  but  deallocator '%s' called at %s(%i)!",
				_duma_allocDesc[slot->allocator].name, slot->filename, slot->lineno,
				_duma_allocDesc[allocator].name, filename, lineno );
		else if ( DUMAFS_DEALLOCATION == slot->fileSource )
			DUMA_Abort("Free mismatch: allocator '%s' used \nbut deallocator '%s' called at %s(%i)!",
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
		DUMA_Print("\nDUMA: Freeing %d bytes at %s(%i) (Allocated from %s(%i)).",
			(DUMA_SIZE)slot->userSize, filename, lineno, slot->filename, slot->lineno);
#else
		DUMA_Print("\nDUMA: Freeing %d bytes.", (DUMA_SIZE)slot->userSize);
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

	internalSizekB = (slot->internalSize+1023) >>10;

	/* protect memory, that nobody can access it */
	/* Free as much protected memory, that we can protect this one */
	/* is there need? and is there a way to free such much? */
	if ( DUMA_PROTECT_FREE > 0L
		&& sumProtectedMem  + internalSizekB >  DUMA_PROTECT_FREE
		&&                    internalSizekB <  DUMA_PROTECT_FREE
		&& sumProtectedMem >= internalSizekB)
	{
		reduceProtectedMemory( internalSizekB );
	}

	if (( EFA_INT_ALLOC != slot->allocator )
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
#ifdef WIN32
		if(slot->stacktrace)
		{
			slot->stacktrace = 0;
			LocalFree(slot->stacktrace);
		}
#endif
	}

	if ( protectAllocList )
	{
		Page_DenyAccess(_duma_allocList, _duma_allocListSize);
		IF__DUMA_INIT_DONE
			DUMA_RELEASE_SEMAPHORE();
	}
}


/*********************************************************/

/* Function: _duma_malloc
 * 
 * A version of malloc.
 */
void * _duma_malloc(size_t size  DUMA_PARAMLIST_FL)
{
	if ( _duma_allocList == 0 )
		_duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */

	return _duma_allocate(0, size, DUMA_PROTECT_BELOW, 
		DUMA_FILL, 1 /*=protectAllocList*/, EFA_MALLOC, 
		DUMA_FAIL_ENV  DUMA_PARAMS_FL);
}


/* end */
