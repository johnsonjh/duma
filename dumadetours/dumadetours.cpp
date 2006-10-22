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

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <detours.h>

#define arrayof(x)		(sizeof(x)/sizeof(x[0]))

int main(int argc, char **argv)
{
	char* pszDllPath = "dumadetoursdll.dll";

	if(getenv("DUMA_DETOURS_DLL"))
		pszDllPath = getenv("DUMA_DETOURS_DLL");

	printf("\n| DUMA -- Detours DUMA Loader\n| Copyright (c) 2006 Michael Eddington\n\n");

	if (argc < 2)
	{
		printf("\nSyntax: dumadetours.exe [command line]\n\n");
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////
		
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char szCommand[2048];
	char szExe[1024];
	char szFullExe[1024] = "\0";
	char* pszFileExe = NULL;
	int cnt = 1;
	
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	
	szCommand[0] = L'\0';
	strcpy(szExe, argv[cnt]);
	for (; cnt < argc; cnt++)
	{
		if (strchr(argv[cnt], ' ') != NULL || strchr(argv[cnt], '\t') != NULL)
		{
			strcat(szCommand, "\"");
			strcat(szCommand, argv[cnt]);
			strcat(szCommand, "\"");
		}
		else
		{
			strcat(szCommand, argv[cnt]);
		}
		
		if (cnt + 1 < argc)
			strcat(szCommand, " ");
	}
	
	printf("dumadetours.exe: Starting: [%s]\n\n", szCommand);
    fflush(stdout);
	
	SetLastError(0);
	SearchPath(NULL, szExe, ".exe", arrayof(szFullExe), szFullExe, &pszFileExe);
	
#ifdef DUMA_DUMA_DETOURS_VERSION == 2.1
    if (!DetourCreateProcessWithDll(szFullExe[0] ? szFullExe : NULL,	// lpApplicationName
                                    szCommand,							// lpCommandLine
									NULL,								// lpProcessAttributes
									NULL,								// lpThreadAttributes
									TRUE,								// bInheritHandles
                                    CREATE_DEFAULT_ERROR_MODE,			// dwCreationFlags
									NULL,								// lpEnvironment
									NULL,								// lpCurrentDirectory
                                    &si,								// lpStartupInfo
									&pi,								// lpProcessInformation
									NULL,								// lpDetouredDllPath
									pszDllPath,							// lpDllName
									NULL))								// pfCreateProcessW
#else
    if (!DetourCreateProcessWithDll(szFullExe[0] ? szFullExe : NULL,
                                    szCommand, NULL, NULL, TRUE,
                                    CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
                                    &si, &pi, pszDllPath, NULL))
#endif
	{
        printf("dumadetours.exe: DetourCreateProcessWithDll failed: %d\n", GetLastError());
        ExitProcess(2);
    }
 	
	WaitForSingleObject(pi.hProcess, INFINITE);
	
	DWORD dwResult = 0;
	if (!GetExitCodeProcess(pi.hProcess, &dwResult))
	{
		printf("dumadetours.exe: GetExitCodeProcess failed: %d\n", GetLastError());
		dwResult = 3;
	}
	
	return dwResult;
}

// end
