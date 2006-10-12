
// Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
// $Id$

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
	OutputDebugString("DumaDetours_HeadCreate");
	return (HANDLE)1;
}

DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapDestroy(HANDLE hHeap),
                  HeapDestroy);

BOOL WINAPI My_HeapDestroy(HANDLE hHeap)
{
	OutputDebugString("DumaDetours_HeapDestroy");
	return TRUE;
}

DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem),
                  HeapFree);

BOOL WINAPI My_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
	char msg[100];
	_snprintf(msg, 99, "DumaDetours_HeapFree %x, %x", hHeap, lpMem);
	OutputDebugString(msg);

	_duma_free(lpMem, __FILE__, __LINE__);
    return TRUE;
}

DETOUR_TRAMPOLINE(LPVOID WINAPI Real_HeapAlloc(HANDLE Heap, DWORD Flags, DWORD Bytes),
                  HeapAlloc);

LPVOID WINAPI My_HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes)
{
	OutputDebugString("DumaDetours_HeapAlloc");
    return _duma_malloc(dwBytes, __FILE__, __LINE__);
}

DETOUR_TRAMPOLINE(LPVOID WINAPI Real_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes),
                  HeapReAlloc);

LPVOID WINAPI My_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
	OutputDebugString("DumaDetours_HeapReAlloc");
    return _duma_realloc(lpMem, dwBytes, __FILE__, __LINE__);
}

///////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
	{
        BOOL bOk;
        PBYTE pbRealTrampoline;
        PBYTE pbRealTarget;

		OutputDebugString("DumaDetours: Installing!");
		
		duma_init();

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
		OutputDebugString("DumaDetours: DLL_PROCESS_DETACH");
        bInternal = TRUE;
        DetourRemove((PBYTE)Real_HeapCreate, (PBYTE)My_HeapCreate);
        DetourRemove((PBYTE)Real_HeapDestroy, (PBYTE)My_HeapDestroy);
        DetourRemove((PBYTE)Real_HeapFree, (PBYTE)My_HeapFree);
        DetourRemove((PBYTE)Real_HeapAlloc, (PBYTE)My_HeapAlloc);
        DetourRemove((PBYTE)Real_HeapReAlloc, (PBYTE)My_HeapReAlloc);
    }

    return TRUE;
}

extern "C" {
    //  Trampolines for SYELOG library.
    //
    DETOUR_TRAMPOLINE(HANDLE WINAPI
                      Real_CreateFileW(LPCWSTR a0, DWORD a1, DWORD a2,
                                       LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5,
                                       HANDLE a6),
                      CreateFileW);

    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_WriteFile(HANDLE hFile,
                                     LPCVOID lpBuffer,
                                     DWORD nNumberOfBytesToWrite,
                                     LPDWORD lpNumberOfBytesWritten,
                                     LPOVERLAPPED lpOverlapped),
                      WriteFile);
    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_FlushFileBuffers(HANDLE hFile),
                      FlushFileBuffers);
    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_CloseHandle(HANDLE hObject),
                      CloseHandle);

    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_WaitNamedPipeW(LPCWSTR lpNamedPipeName, DWORD nTimeOut),
                      WaitNamedPipeW);
    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_SetNamedPipeHandleState(HANDLE hNamedPipe,
                                                   LPDWORD lpMode,
                                                   LPDWORD lpMaxCollectionCount,
                                                   LPDWORD lpCollectDataTimeout),
                      SetNamedPipeHandleState);

    DETOUR_TRAMPOLINE(DWORD WINAPI
                      Real_GetCurrentProcessId(VOID),
                      GetCurrentProcessId);
    DETOUR_TRAMPOLINE(VOID WINAPI
                      Real_GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime),
                      GetSystemTimeAsFileTime);

    DETOUR_TRAMPOLINE(VOID WINAPI
                      Real_InitializeCriticalSection(LPCRITICAL_SECTION lpSection),
                      InitializeCriticalSection);
    DETOUR_TRAMPOLINE(VOID WINAPI
                      Real_EnterCriticalSection(LPCRITICAL_SECTION lpSection),
                      EnterCriticalSection);
    DETOUR_TRAMPOLINE(VOID WINAPI
                      Real_LeaveCriticalSection(LPCRITICAL_SECTION lpSection),
                      LeaveCriticalSection);
}

// end
