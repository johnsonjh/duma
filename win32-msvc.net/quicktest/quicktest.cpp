// quicktest.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "duma.h"

int _tmain(int argc, _TCHAR* argv[])
{
	fprintf(stderr, "!!!!starting!!!\n");

	for(int i=0; i<100; i++)
	{
		fprintf(stderr, ".");
		char* tmp = (char*) malloc(100);
		//tmp[110] = 'a';
		free(tmp);
	}

#if 1

	for(int i=0; i<100; i++)
	{
		fprintf(stderr, ".");
		char* tmp = (char*) malloc(200);
		//tmp[110] = 'a';
		//free(tmp);
	}
#endif

	fprintf(stderr, "QUICKTEST DONE\n");

	return 0;
}

