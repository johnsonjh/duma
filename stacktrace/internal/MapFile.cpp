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

#include "MapFile.h"
#include "MapFileEntry.h"
#include "TextFile.h"
#include "Array.h"
#include <algorithm>
#include <string.h>
#include <ctype.h>
#ifdef WIN32
#include <windows.h>
#endif

//-----------------------------------------------------------------------------

namespace dev
{

// Lots of changes from origional to fix bugs
// improve spead, and clean things up.
// We are only parsing function entries now, but
// should add support for line numbers.
class MapFile::MapFileImpl
{
public:
	long				loadAddr;
	char				name[256];
	Array<MapFileEntry*> entries;

	MapFileImpl( const char* filename ) :
		loadAddr(0), m_file( filename ), m_err( MapFile::ERROR_NONE )
	{
		m_file.readString( name, sizeof(name) );

		char buf[1024];
		while ( m_file.readString(buf,sizeof(buf)) )
		{
			if ( !strcmp("Address",buf) )
				parseEntries();
			else
				m_file.skipLine();
		}

		std::sort( entries.begin(), entries.end() );
	}

	~MapFileImpl()
	{
	}

	ErrorType error() const
	{
		if ( m_err != MapFile::ERROR_NONE )
			return m_err;

		switch ( m_file.error() )
		{
		case TextFile::ERROR_OPEN:	return MapFile::ERROR_OPEN;
		case TextFile::ERROR_READ:	return MapFile::ERROR_READ;
		case TextFile::ERROR_PARSE:	return MapFile::ERROR_PARSE;
		default:					return MapFile::ERROR_NONE;
		}
	}

	int line() const
	{
		if ( m_err != MapFile::ERROR_NONE )
			return m_errLine;

		return m_file.line();
	}
	
private:
	TextFile			m_file;
	MapFile::ErrorType	m_err;
	int					m_errLine;

	/**
	 * Returns true if the next line is empty.
	 */
	bool nextLineEmpty()
	{
		m_file.skipLine();
		char ch;
		while ( m_file.peekChar(&ch) && isspace(ch) && ch != '\n' )
			m_file.readChar( &ch );
		if ( m_file.peekChar(&ch) && ch == '\n' )
			return true;
		return false;
	}

	/** 
	 * Parses specified string. 
	 * Sets error if parsed string doesnt match. 
	 */
	void parse( const char* str )
	{
		char buf[256];
		m_file.readString( buf, sizeof(buf) );
		if ( strcmp(str,buf) )
		{
			m_err = MapFile::ERROR_PARSE;
			m_errLine = m_file.line();
		}
	}

	/**
	 * Parses specified character. 
	 * Sets error if parsed character doesnt match. 
	 */
	void parse( char ch )
	{
		char ch2;
		if ( !m_file.readChar(&ch2) || ch2 != ch )
		{
			m_err = MapFile::ERROR_PARSE;
			m_errLine = m_file.line();
		}
	}

	/**
	 * Example:
	 * (Preferred) load address is 00400000
	 */
	void parseLoadAddress()
	{
		parse( "load" ); parse( "address" ); parse( "is" );
		loadAddr = m_file.readHex();
	}

	/**
	 * Example:
	 * (Address)       Publics by Value           Rva+Base     Lib:Object
	 * 0001:000001a0   ?stackTrace@@YAXXZ         004011a0 f   main.obj
	 */
	void parseEntries()
	{
		unsigned int seg;
		unsigned int offs;
		unsigned int rvabase;
		char	buf[256];
		char*	entryname;
		char	lib[256];
		char*	str;

		parse( "Publics" ); parse( "by" ); parse( "Value" );
		parse( "Rva+Base" );
		parse( "Lib:Object" );
		m_file.skipWhitespace();
		
		while ( !error() )
		{
			seg = m_file.readHex();
			parse( ':' );
			offs = m_file.readHex();
			m_file.readString( buf, sizeof(buf) );
			entryname = buf;
			rvabase = m_file.readHex();
			m_file.readString( lib, sizeof(lib) );
			if(!strcmp(lib, "f"))
				m_file.readString( lib, sizeof(lib) );

			// chop entry name at @@
			str = strstr( entryname, "@@" );
			if ( str )
				*str = 0;

			// skip preceding ?01..
			while ( isdigit(*entryname) || *entryname == '?' || *entryname == '$' )
				++entryname;

			// conv @ -> .
			for ( str = entryname ; *str ; ++str )
				if ( *str == '@' )
					*str = '.';

			entries.add( new MapFileEntry(seg,offs,0,entryname, rvabase, lib) );

			// break at empty line
			if ( nextLineEmpty() )
				break;
		}
	}
};

//-----------------------------------------------------------------------------

MapFile::MapFile( const char* filename )
{
	m_this = new MapFileImpl( filename );
}

MapFile::~MapFile()
{
	delete m_this;
}

long MapFile::loadAddress() const
{
	return m_this->loadAddr;
}

MapFileEntry*	MapFile::getEntry( int i ) const
{
	return m_this->entries[i];
}

int MapFile::entries() const
{
	return m_this->entries.size();
}

MapFile::ErrorType MapFile::error() const
{
	return m_this->error();
}

int MapFile::line() const
{
	return m_this->line();
}

int MapFile::findEntry( long addr ) const
{
	// Changed this to use the rvabase instead
	// of calculating things based on segment.

	// Addresses of zero are bogus
	if(addr == 0)
		return -1;

	// Check and see if our addr is way larger then
	// the highest rva+base address we have.
	if(addr > (getEntry(entries()-1)->rvabase() + 10000))
		return -1;
	
	// Entries are sorted, so searching from last
	// to first will work well
	for ( int i = entries()-1 ; i >= 0  ; --i )
	{
		if(getEntry( i )->rvabase() <= addr)
			return i;
	}

	return -1;
}

void MapFile::getModuleMapFilename( char* buffer, int bufferSize )
{
	int len = 0;
	buffer[len] = 0;

#ifdef WIN32
	// get name of the exe/dll
	len = GetModuleFileName( GetModuleHandle(0), buffer, bufferSize-1 );
	buffer[len] = 0;
#endif

	// remove .exe or .dll extension
	if ( len > 3 && 
		(!strcmp(buffer+len-4,".exe") || !strcmp(buffer+len-4,".EXE") || 
		!strcmp(buffer+len-4,".DLL") || !strcmp(buffer+len-4,".dll")) )
	{
		buffer[len-4] = 0;
	}

	// append .map extension
	if ( (int)strlen(buffer)+4 < bufferSize )
	{
		strcat( buffer, ".map" );
	}
}


} // dev
