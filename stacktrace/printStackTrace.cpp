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

// $Id$


#include "printStackTrace.h"
#include "StackTrace.h"
#include "MapFile.h"
#include <stdio.h>
#include <string.h>

//-----------------------------------------------------------------------------

using namespace dev;

//-----------------------------------------------------------------------------

// We will keep the parsed map file around
// after we create it.
static MapFile*	map = NULL;


// Clean up any used memory
extern "C" void StackTraceCleanup()
{
	delete map;
	map = NULL;
}

/**
 * Prints stack trace to user defined buffer.
 * Always terminates the buffer with 0.
 *
 * TODO: Support for multiple map files in func def.
 *       already supported by core code.
 */
extern "C" void printStackTrace( char* buffer, int bufferSize, char* mapFilename )
{
	if(map == NULL)
	{
		// find out map file name
		char modname[500];
		if(mapFilename == NULL)
			MapFile::getModuleMapFilename( modname, sizeof(modname) );
		else
			strncpy(modname, mapFilename, sizeof(modname)/sizeof(char));

		// parse map file
		map = new MapFile( modname );
		switch ( map->error() )
		{
		case MapFile::ERROR_OPEN:	sprintf( buffer, "Failed to open map file %s\n", modname ); break;
		case MapFile::ERROR_READ:	sprintf( buffer, "Error while reading map file %s(%i)\n", modname, map->line() ); break;
		case MapFile::ERROR_PARSE:	sprintf( buffer, "Parse error in map file %s(%i)\n", modname, map->line() ); break;
		default:					break;
		}
	}

	// print stack trace to buffer
	if ( !map->error() )
	{
		MapFile* maps[] = {map};
		StackTrace::printStackTrace( maps, 1, 1, 16, buffer, bufferSize);
	}
}
