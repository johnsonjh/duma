
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

#ifndef DUMA_PAGING_H
#define DUMA_PAGING_H

#include "log.h"

/*
 * Lots of systems are missing the definition of PROT_NONE.
 */
#ifndef  PROT_NONE
#define  PROT_NONE  0
#endif

static caddr_t  startAddr = (caddr_t) 0;

/* Function: mprotectFailed
 *
 * Report that VirtualProtect or mprotect failed and abort
 * program execution.
 */
static void mprotectFailed(void)
{
#if defined(WIN32)
	DUMA_Abort("VirtualProtect() failed: %s", stringErrorReport());
#else
	DUMA_Abort("mprotect() failed: %s", stringErrorReport());
#endif
}


/* Function: Page_Create
 *
 * Create memory.  Allocates actual memory. Uses
 * VirtualAlloc on windows and mmap on unix.
 *
 * flags - Passed along from kmalloc (GFP_ATOMIC and the like)
 *
 * See Also: 
 *	<Page_Delete>
 */
static void *
Page_Create(size_t size, int exitonfail, int printerror, int flags)
{
	caddr_t    allocation;
	
	unsigned long numPages = size/DUMA_PAGE_SIZE;
	if( size % DUMA_AGE_SIZE )
		numPages++;
	
	unsigned long order = ilog2(numPages);
	
	allocation = __get_free_pages(flags, order);
	
	if ( allocation == 0 )
	{
		if ( exitonfail )
			DUMA_Abort("__get_fre_pages(%d, %d) failed: %s", flags, order, stringErrorReport());
		else if ( printerror )
			DUMA_Print("\nDUMA warning: __get_fre_pages(%d, %d) failed: %s", flags, order, stringErrorReport());
	}
	
	return (void *)allocation;
}


/* Function: Page_AllowAccess
 *
 * Allow memory access to allocated memory.
 *
 * See Also: 
 *	<Page_DenyAccess>
 */
void Page_AllowAccess(void * address, size_t size)
{
	if ( mprotect((caddr_t)address, size, PROT_READ|PROT_WRITE) < 0 )
		mprotectFailed();
#endif
}


/* Function: Page_DenyAccess
 *
 * Deny access to allocated memory region.
 *
 * See Also: 
 *	<Page_AllowAccess>
 */
static void Page_DenyAccess(void * address, size_t size)
{
#if defined(WIN32)
  SIZE_T OldProtect, retQuery;
  MEMORY_BASIC_INFORMATION MemInfo;
  size_t tail_size;
  BOOL ret;

  while (size >0)
  {
    retQuery = VirtualQuery(address, &MemInfo, sizeof(MemInfo));
    if (retQuery < sizeof(MemInfo))
      DUMA_Abort("VirtualQuery() failed\n");
    tail_size = (size > MemInfo.RegionSize) ? MemInfo.RegionSize : size;
    ret = VirtualProtect(
                          (LPVOID) address        /* address of region of committed pages */
                        , (DWORD) tail_size       /* size of the region */
                        , (DWORD) PAGE_NOACCESS   /* desired access protection */
                        , (PDWORD) &OldProtect    /* address of variable to get old protection */
                        );
    if (0 == ret)
      mprotectFailed();

    address = ((char *)address) + tail_size;
    size -= tail_size;
  }

#else
  if ( mprotect((caddr_t)address, size, PROT_NONE) < 0 )
    mprotectFailed();
#endif
}

extern struct _DUMA_Slot;


/* Function: Page_Delete
 *
 * Free's DUMA allocated memory.  This is the real deal, make sure
 * the page is no longer in our slot list first!
 *
 * See Also:
 *	<Page_Create>
 */
static void Page_Delete(void * address, size_t size)
{
#if defined(WIN32)

	void * alloc_address  = address;
	size_t alloc_size     = size;
	SIZE_T retQuery;
	MEMORY_BASIC_INFORMATION MemInfo;
	BOOL ret;

	/* release physical memory commited to virtual address space */
	while (size >0)
	{
		retQuery = VirtualQuery(address, &MemInfo, sizeof(MemInfo));

		if (retQuery < sizeof(MemInfo))
			DUMA_Abort("VirtualQuery() failed\n");

		if ( MemInfo.State == MEM_COMMIT )
		{
			ret = VirtualFree(
				(LPVOID) MemInfo.BaseAddress /* base of committed pages */
				, (DWORD) MemInfo.RegionSize   /* size of the region */
				, (DWORD) MEM_DECOMMIT         /* type of free operation */
				);

			if (0 == ret)
				DUMA_Abort("VirtualFree(,,MEM_DECOMMIT) failed: %s", stringErrorReport());
		}

		address = ((char *)address) + MemInfo.RegionSize;
		size -= MemInfo.RegionSize;
	}

	/* release virtual address space */
	ret = VirtualFree(
		(LPVOID) alloc_address
		, (DWORD) 0
		, (DWORD) MEM_RELEASE
		);

	if (0 == ret)
		DUMA_Abort("VirtualFree(,,MEM_RELEASE) failed: %s", stringErrorReport());

#else

	if ( munmap((caddr_t)address, size) < 0 )
		Page_DenyAccess(address, size);

#endif
}


/* Function: Page_Size
 *
 * Retrieve page size.
 */
static size_t
Page_Size(void)
{
#if defined(WIN32)
  SYSTEM_INFO SystemInfo;
  GetSystemInfo( &SystemInfo );
  return (size_t)SystemInfo.dwPageSize;
#elif defined(_SC_PAGESIZE)
	return (size_t)sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
	return (size_t)sysconf(_SC_PAGE_SIZE);
#else
/* extern int	getpagesize(); */
	return getpagesize();
#endif
}


#endif /* DUMA_PAGING_H */
