/*
 * Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
 *
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
 */

// $Id$

// This implements a detours DLL that will take over the HeapXxxx
// functions and use DUMA instead to track allocations.  Other
// memory functions will also be hooked in the future.
//
// Currently we will only intercept memory being allocated for heaps
// that were created for HeapCreate calls we intercept.  This is done
// by always returning 1 for HeapCreate.  Any other heap handle will
// be passed to the real HeapXxxx functions.
//
// It is very usefull to have a map file when using duma + detours and
// enable stacktrace output (DUMA_OUTPUT_STACKTRACE, and 
// DUMA_OUTPUT_STACKTRACE_MAPFILE) as you will not have filename's in 
// the output.

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include "detours.h"

#include "duma.h"

#pragma warning(disable:4100)   // Trampolines don't use formal parameters.

//////////////////////////////////////////////////////////////////////////////

static BOOL bInternal = TRUE;

DETOUR_TRAMPOLINE(HANDLE WINAPI Real_HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize), 
				  HeapCreate);

HANDLE WINAPI My_HeapCreate( DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize )
{
	return (HANDLE)1;
}

DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapDestroy(HANDLE hHeap),
                  HeapDestroy);

BOOL WINAPI My_HeapDestroy(HANDLE hHeap)
{
	if(hHeap != (HANDLE)1)
		return Real_HeapDestroy(hHeap);

	return TRUE;
}

DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem),
                  HeapFree);

BOOL WINAPI My_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
	if(hHeap != (HANDLE)1)
		return Real_HeapFree(hHeap, dwFlags, lpMem);

	_duma_free(lpMem, __FILE__, __LINE__);
    return TRUE;
}

DETOUR_TRAMPOLINE(LPVOID WINAPI Real_HeapAlloc(HANDLE Heap, DWORD Flags, DWORD Bytes),
                  HeapAlloc);

LPVOID WINAPI My_HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes)
{
	if(hHeap != (HANDLE)1)
		return Real_HeapAlloc(hHeap, dwFlags, dwBytes);

    return _duma_malloc(dwBytes, __FILE__, __LINE__);
}

DETOUR_TRAMPOLINE(LPVOID WINAPI Real_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes),
                  HeapReAlloc);

LPVOID WINAPI My_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
	if(hHeap != (HANDLE)1)
		return Real_HeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);

    return _duma_realloc(lpMem, dwBytes, __FILE__, __LINE__);
}

DETOUR_TRAMPOLINE(SIZE_T WINAPI Real_HeapSize(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem),
                  HeapSize);

SIZE_T WINAPI My_HeapSize(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
	if(hHeap != (HANDLE)1)
		return Real_HeapSize(hHeap, dwFlags, lpMem);

    return sizeof(lpMem);
}


DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem),
				  HeapValidate);

BOOL WINAPI My_HeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
	if(hHeap != (HANDLE)1)
		return Real_HeapValidate(hHeap, dwFlags, lpMem);

	return true;
}

///////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// For debugging.  Allows us time to attach a debugger.
		//fprintf(stderr, "DUMA: Press any key to continue...");
		//fgetc(stdin);
		
		duma_init();
		
		DetourFunctionWithTrampoline((PBYTE)Real_HeapCreate,	(PBYTE)My_HeapCreate);
		DetourFunctionWithTrampoline((PBYTE)Real_HeapDestroy,	(PBYTE)My_HeapDestroy);
		DetourFunctionWithTrampoline((PBYTE)Real_HeapFree,		(PBYTE)My_HeapFree);
		DetourFunctionWithTrampoline((PBYTE)Real_HeapAlloc,		(PBYTE)My_HeapAlloc);
		DetourFunctionWithTrampoline((PBYTE)Real_HeapReAlloc,	(PBYTE)My_HeapReAlloc);
		DetourFunctionWithTrampoline((PBYTE)Real_HeapSize,		(PBYTE)My_HeapSize);
		DetourFunctionWithTrampoline((PBYTE)Real_HeapValidate,	(PBYTE)My_HeapValidate);
		
		bInternal = FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
        bInternal = TRUE;
		
        DetourRemove((PBYTE)Real_HeapCreate,	(PBYTE)My_HeapCreate);
        DetourRemove((PBYTE)Real_HeapDestroy,	(PBYTE)My_HeapDestroy);
        DetourRemove((PBYTE)Real_HeapFree,		(PBYTE)My_HeapFree);
        DetourRemove((PBYTE)Real_HeapAlloc,		(PBYTE)My_HeapAlloc);
        DetourRemove((PBYTE)Real_HeapReAlloc,	(PBYTE)My_HeapReAlloc);
		DetourRemove((PBYTE)Real_HeapSize,		(PBYTE)My_HeapSize);
		DetourRemove((PBYTE)Real_HeapValidate,	(PBYTE)My_HeapValidate);
    }
	
    return TRUE;
}

// end
