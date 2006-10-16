/*
 * Copyright (c) 2006 Michael Eddington
 * Copyright (c) 2001 Jani Kajala
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * Jani Kajala makes no representations about the suitability 
 * of this software for any purpose. It is provided "as is" 
 * without express or implied warranty.
 */

#include "StackTrace.h"
#include "MapFile.h"
#include "MapFileEntry.h"
#include <string.h>
#include <stdio.h>

//-----------------------------------------------------------------------------

#define MAX_DEPTH 32

//-----------------------------------------------------------------------------

namespace dev
{


/*
EBP points to stack frame (at least 2 DWORDs) which looks something like this:
DWORD[EBP+4] = return address (=calling function)
DWORD[EBP+0] = calling function stack frame

So we 
1) get the EBP to EBX 
2) take calling function address to EAX
3) parent stack frame to EBX and 
4) loop 2,3,4 until desired level in calling stack is reached.

Beware that compiler does not always make stack frame (at least if s.c. "omit stack frame" 
optimization is enabled), so at least I'd use this code only in debug build.
*/
static long getCaller( int index )
{
#if defined(_DEBUG) && defined(_MSC_VER) && defined(_M_IX86)

	// Changed a couple things here to prevent some
	// access violations, esp with release code that
	// doesn't always have a stack frame.
	//
	// 1. Check next frame to see if we are below 10000
	// 2. If we are forcing a stop make the return addr 0

	long caller = 0;
	__asm
	{
		mov ebx, ebp
		mov ecx, index
		inc ecx
		xor eax, eax
StackTrace_getCaller_next:
		mov eax, [ebx+4]
		mov ebx, [ebx]
		cmp ebx, 10000
		jb StackTrace_getCaller_stop
		dec ecx
		jnz StackTrace_getCaller_next
		jmp StackTrace_getCaller_end
StackTrace_getCaller_stop:
		mov eax, 0
StackTrace_getCaller_end:
		mov caller, eax
	}
	return caller;

#else

	return 0;

#endif
}

int StackTrace::printStackTrace( MapFile** map, int maps,
	int initLevel, int maxDepth, char* buffer, int bufferSize )
{
	if ( maxDepth > MAX_DEPTH )
		maxDepth = MAX_DEPTH;

	// list callers
	long callersAddr[MAX_DEPTH];
	MapFileEntry* callersEntry[MAX_DEPTH];
	int callers;
	int i;
	int entry;
	long addr = -1;
	int flag = 0;
	int j;

	for (callers = 0, i = initLevel ; i < maxDepth && addr ; i++)
	{
		addr = getCaller( i );

		callersEntry[callers] = NULL;
		for (entry = -1, j = 0 ; j < maps ; ++j )
		{
			entry = map[j]->findEntry( addr );

			if ( entry != -1 )
			{
				callersEntry[callers] = map[j]->getEntry(entry);
				break;
			}
		}

		if(entry == -1)
			continue;
		
		callersAddr[callers] = addr;

		callers++;
	}

	// output call stack
	if ( bufferSize > 0 )
		*buffer = 0;

	int needed = 0;

	for ( i = 1 ; i <= callers ; ++i )
	{
		addr = callersAddr[callers-i];

		// format entry to tempory buf
		char buf[MapFileEntry::MAX_NAME+MAX_DEPTH+20];	// name + margin + hex number
		buf[0] = 0;

		for ( int k = initLevel-1 ; k < i ; ++k )
			strcat( buf, " " );

		if ( callersEntry[callers-i] == NULL )
			sprintf( buf+strlen(buf), "0x%x\n", addr );
		else
			sprintf( buf+strlen(buf), "%s (%x)\n", callersEntry[callers-i]->name(), addr );

		// append temporary buf to output buffer if space left
		needed += strlen( buf );
		if ( needed < bufferSize )
			strcat( buffer, buf );
	}

	// terminate output buffer
	if ( needed < bufferSize )
		buffer[needed] = 0;
	else if ( bufferSize > 0 )
		buffer[bufferSize-1] = 0;

	return needed;
}


} // dev
