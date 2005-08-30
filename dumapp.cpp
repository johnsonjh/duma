
/*
 * DUMA - Red-Zone memory allocator.
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
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
 *
 *
 * FILE CONTENTS:
 * internal implementation file
 * replaces various global C++ new/delete/new[]/delete[] operators
 */

#ifndef DUMA_NO_CPP_SUPPORT


#include <new>

#include <stdlib.h>

#include "duma.h"


#if WIN32
#define DUMA_CDECL  __cdecl
#else
#define DUMA_CDECL
#endif

#ifdef _MSC_VER
#define DUMA_SIZE_T    size_t
#else
#define DUMA_SIZE_T    std::size_t
#endif


static const char unknown_cxx_file[] =
 "UNKNOWN (use #include \"dumapp.h\")";


#ifndef DUMA_NO_LEAKDETECTION
#define DUMA_PARAMS_UK          , unknown_cxx_file, 0
#else
#define DUMA_PARAMS_UK
#endif


/********************************************************************
********************************************************************/

/* 1x : SINGLE OBJECT FORM - NO DEBUG INFORMATION */

/* (11) = (a) ; ASW */
void * DUMA_CDECL operator new( DUMA_SIZE_T size )
throw(std::bad_alloc)
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  void *ptr = _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_NEW_ELEM, DUMA_FAIL_NULL  DUMA_PARAMS_UK);
  if (!ptr) throw std::bad_alloc();
  return ptr;
}


/* (12) = (b) ; ASN */
void * DUMA_CDECL operator new( DUMA_SIZE_T size, const std::nothrow_t & )
throw()
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  void *ptr = _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_NEW_ELEM, DUMA_FAIL_NULL  DUMA_PARAMS_UK);
  return ptr;
}


/* (13) = (c) ; FSW */
void   DUMA_CDECL operator delete( void *ptr )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM  DUMA_PARAMS_UK);
}


/* (14) = (d) ; FSN */
void   DUMA_CDECL operator delete( void * ptr, const std::nothrow_t & )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM  DUMA_PARAMS_UK);
}



/********************************************************************
********************************************************************/

/* 2x : ARRAY OBJECT FORM - NO DEBUG INFORMATION */


/* (21) = (a) ; AAW */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size )
throw(std::bad_alloc)
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  void *ptr = _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_NEW_ARRAY, DUMA_FAIL_NULL  DUMA_PARAMS_UK);
  if (!ptr) throw std::bad_alloc();
  return ptr;
}


/* (22) = (b) ; AAN */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size, const std::nothrow_t & )
throw()
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  void *ptr = _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_NEW_ARRAY, DUMA_FAIL_NULL  DUMA_PARAMS_UK);
  return ptr;
}


/* (23) = (c) ; FAW */
void   DUMA_CDECL operator delete[]( void * ptr )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY  DUMA_PARAMS_UK);
}


/* (24) = (d) ; FAN */
void   DUMA_CDECL operator delete[]( void * ptr, const std::nothrow_t & )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY  DUMA_PARAMS_UK);
}



/********************************************************************
********************************************************************/

#ifndef DUMA_NO_LEAKDETECTION

/* 3x : SINGLE OBJECT FORM - WITH DEBUG INFORMATION */


/* (31) = (a) ; ASW */
void * DUMA_CDECL operator new( DUMA_SIZE_T size, const char *filename, int lineno )
throw( std::bad_alloc )
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  void *ptr = _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_NEW_ELEM, DUMA_FAIL_NULL, filename, lineno);
  if (!ptr) throw std::bad_alloc();
  return ptr;
}

/* (32) = (b) ; ASN */
void * DUMA_CDECL operator new( DUMA_SIZE_T size, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  void *ptr = _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_NEW_ELEM, DUMA_FAIL_NULL, filename, lineno);
  return ptr;
}


/* (33) = (c) ; FSW */
void   DUMA_CDECL operator delete( void *ptr, const char *filename, int lineno )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM, filename, lineno);
}


/* (34) = (d) ; FSN */
void   DUMA_CDECL operator delete( void *ptr, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM, filename, lineno);
}


/********************************************************************
********************************************************************/

/* 4x : ARRAY OBJECT FORM - WITH DEBUG INFORMATION */


/* (41) = (a) ; AAW */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size, const char *filename, int lineno )
throw( std::bad_alloc )
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  void *ptr = _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_NEW_ARRAY, DUMA_FAIL_NULL, filename, lineno);
  if (!ptr) throw std::bad_alloc();
  return ptr;
}


/* (42) = (b) ; AAN */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  if ( _duma_allocList == 0 )  _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */
  void *ptr = _duma_allocate(DUMA_ALIGNMENT, size, DUMA_PROTECT_BELOW, DUMA_FILL, 1 /*=protectAllocList*/, EFA_NEW_ARRAY, DUMA_FAIL_NULL, filename, lineno);
  return ptr;
}


/* (43) = (c) ; FAW */
void   DUMA_CDECL operator delete[]( void *ptr, const char *filename, int lineno )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY, filename, lineno);
}


/* (44) = (d) ; FAN */
void   DUMA_CDECL operator delete[]( void *ptr, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY, filename, lineno);
}

#endif /* end ifdef DUMA_NO_LEAKDETECTION */

#endif /* DUMA_NO_CPP_SUPPORT */

