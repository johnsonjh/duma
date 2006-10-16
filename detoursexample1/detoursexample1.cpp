// detoursexample1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

int _tmain(int argc, _TCHAR* argv[])
{
	fprintf(stderr, "!!!!detoursexample1 starting!!!\n");
	printf("!!!!detoursexample1 starting!!!\n");
	fflush(stdout);
	OutputDebugString("!!!! ITS ME !!!!");
	for(int i=0; i<100; i++)
	{
		fprintf(stderr, ".");
		char* tmp = (char*) malloc(100);
		//tmp[110] = 'a';
		//free(tmp);
	}

/*	HANDLE h = HeapCreate(NULL, 1024, 2024);
	for(int i=0; i<100; i++)
	{
		fprintf(stderr, "+");
		char* tmp = (char*) HeapAlloc(h, 0, 100);
		HeapFree(h, 0, tmp);
	}
	HeapDestroy(h);
*/
	fprintf(stderr, "detoursexample1 done\n");

	return 0;
}

