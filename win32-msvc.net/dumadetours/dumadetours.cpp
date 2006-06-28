
// Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
// $Id$

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <detours.h>

#define arrayof(x)		(sizeof(x)/sizeof(x[0]))

int main(int argc, char **argv)
{
	char* pszDllPath = "dumadetoursdll.dll";

	printf("| DUMA -- Detours DUMA Loader\n| Copyright (c) 2006 Michael Eddington\n\n");

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
	
    if (!DetourCreateProcessWithDll(szFullExe[0] ? szFullExe : NULL,
                                    szCommand, NULL, NULL, TRUE,
                                    CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
                                    &si, &pi, pszDllPath, NULL))
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
