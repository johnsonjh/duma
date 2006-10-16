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

#include "MapFileEntry.h"
#include <string.h>

//-----------------------------------------------------------------------------

namespace dev
{


MapFileEntry::MapFileEntry()
{
	m_sec = 0;
	m_addr = 0; 
	m_len = 0;
	m_name[0] = 0;
}

MapFileEntry::MapFileEntry( long section, long offset, long length, 
							const char* name, long rvabase, const char* lib )
{
	m_sec = section;
	m_addr = offset;
	m_len = length;
	m_rvabase = rvabase;

	strncpy( m_name, name, MAX_NAME ); 
	m_name[MAX_NAME] = 0;
	if(lib)
		strncpy( m_lib, lib, MAX_NAME ); 
	m_name[MAX_NAME] = 0;
}

long MapFileEntry::section() const
{
	return m_sec;
}

long MapFileEntry::offset() const
{
	return m_addr;
}

long MapFileEntry::length() const
{
	return m_len;
}

const char* MapFileEntry::name() const
{
	return m_name;
}
/** Returns rva+base */
long MapFileEntry::rvabase() const
{
	return m_rvabase;
}

/** Returns name of the library */
const char*	MapFileEntry::lib() const
{
	return m_lib;
}

bool MapFileEntry::operator<( const MapFileEntry& other ) const
{
	return m_rvabase < other.m_rvabase;
}


} // dev
