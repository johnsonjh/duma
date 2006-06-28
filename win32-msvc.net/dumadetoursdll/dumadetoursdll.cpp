
// Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
// $Id$

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include "detours.h"

#include "duma.h"

#pragma warning(disable:4100)   // Trampolines don't use formal parameters.

static void Print(const char* msg)
{
	FILE* fd;

	fd = fopen("c:\\log.txt", "ab+");
	fprintf(fd, "%s", msg);
	fclose(fd);
}

//////////////////////////////////////////////////////////////////////////////

static BOOL bInternal = TRUE;

DETOUR_TRAMPOLINE(HANDLE WINAPI Real_HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize), 
				  HeapCreate);

HANDLE WINAPI My_HeapCreate( DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize )
{
	Print("My_HeadCreate\n");
    HANDLE pvRet = Real_HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
    return pvRet;
}

DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapDestroy(HANDLE hHeap),
                  HeapDestroy);

BOOL WINAPI My_HeapDestroy(HANDLE hHeap)
{
	Print("My_HeapDestroy\n");
    BOOL pvRet = Real_HeapDestroy(hHeap);
    return pvRet;
}

DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem),
                  HeapFree);

BOOL WINAPI My_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
	Print("My_HeapFree\n");
    BOOL pvRet = Real_HeapFree(hHeap, dwFlags, lpMem);
    return pvRet;
}

DETOUR_TRAMPOLINE(LPVOID WINAPI Real_HeapAlloc(HANDLE Heap, DWORD Flags, DWORD Bytes),
                  HeapAlloc);

LPVOID WINAPI My_HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes)
{
	Print("My_HeapAlloc\n");
    LPVOID pvRet = Real_HeapAlloc(hHeap, dwFlags, dwBytes);
    return pvRet;
}

DETOUR_TRAMPOLINE(LPVOID WINAPI Real_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes),
                  HeapReAlloc);

LPVOID WINAPI My_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
	Print("My_HeapReAlloc\n");
    LPVOID pvRet = Real_HeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);
    return pvRet;
}

///////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
	{
		Print("dumadetoursdll.dll: DllMain: DLL Process Attach\n");
        BOOL bOk;
        PBYTE pbRealTrampoline;
        PBYTE pbRealTarget;

        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapCreate, (PBYTE)My_HeapCreate,
                                             &pbRealTrampoline, &pbRealTarget);
        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapDestroy, (PBYTE)My_HeapDestroy,
                                             &pbRealTrampoline, &pbRealTarget);
        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapFree, (PBYTE)My_HeapFree,
                                             &pbRealTrampoline, &pbRealTarget);
        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapAlloc, (PBYTE)My_HeapAlloc,
                                             &pbRealTrampoline, &pbRealTarget);
        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapReAlloc, (PBYTE)My_HeapReAlloc,
                                             &pbRealTrampoline, &pbRealTarget);

        bInternal = FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
		Print("dumadetoursdll.dll: DllMain: DLL Process Detach\n");
        bInternal = TRUE;
        DetourRemove((PBYTE)Real_HeapCreate, (PBYTE)My_HeapCreate);
        DetourRemove((PBYTE)Real_HeapDestroy, (PBYTE)My_HeapDestroy);
        DetourRemove((PBYTE)Real_HeapFree, (PBYTE)My_HeapFree);
        DetourRemove((PBYTE)Real_HeapAlloc, (PBYTE)My_HeapAlloc);
        DetourRemove((PBYTE)Real_HeapReAlloc, (PBYTE)My_HeapReAlloc);
    }

    return TRUE;
}


// end
