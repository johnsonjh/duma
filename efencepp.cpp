
/*
 * Electric Fence - Red-Zone memory allocator.
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

#ifndef EF_NO_CPP_SUPPORT


#include <new>

#include <stdlib.h>

#include "efence.h"


#if WIN32
#define EF_CDECL  __cdecl
#else
#define EF_CDECL
#endif

#ifdef _MSC_VER
#define EF_SIZE_T    size_t
#else
#define EF_SIZE_T    std::size_t
#endif


static const char unknown_cxx_file[] =
 "UNKNOWN (use #include \"efencepp.h\")";


/********************************************************************
********************************************************************/

/* 1x : SINGLE OBJECT FORM - NO DEBUG INFORMATION */

/* (11) = (a) ; ASW */
void * EF_CDECL operator new( EF_SIZE_T size )
throw(std::bad_alloc)
{
#ifndef EF_NO_LEAKDETECTION
  void *ptr = _eff_malloc(size, EFST_ALLOC_NEW_ELEM, unknown_cxx_file, 0);
#else
  void *ptr = _eff_malloc(size);
#endif
  if (!ptr) throw std::bad_alloc();
  return ptr;
}


/* (12) = (b) ; ASN */
void * EF_CDECL operator new( EF_SIZE_T size, const std::nothrow_t & )
throw()
{
#ifndef EF_NO_LEAKDETECTION
  void *ptr = _eff_malloc(size, EFST_ALLOC_NEW_ELEM, unknown_cxx_file, 0);
#else
  void *ptr = _eff_malloc(size);
#endif
  return ptr;
}


/* (13) = (c) ; FSW */
void   EF_CDECL operator delete( void *ptr )
throw()
{
#ifndef EF_NO_LEAKDETECTION
  _eff_free(ptr, EFST_ALLOC_NEW_ELEM);
#else
  _eff_free(ptr);
#endif
}


/* (14) = (d) ; FSN */
void   EF_CDECL operator delete( void * ptr, const std::nothrow_t & )
throw()
{
#ifndef EF_NO_LEAKDETECTION
  _eff_free(ptr, EFST_ALLOC_NEW_ELEM);
#else
  _eff_free(ptr);
#endif
}



/********************************************************************
********************************************************************/

/* 2x : ARRAY OBJECT FORM - NO DEBUG INFORMATION */


/* (21) = (a) ; AAW */
void * EF_CDECL operator new[]( EF_SIZE_T size )
throw(std::bad_alloc)
{
#ifndef EF_NO_LEAKDETECTION
  void *ptr = _eff_malloc(size, EFST_ALLOC_NEW_ARRAY, unknown_cxx_file, 0);
#else
  void *ptr = _eff_malloc(size);
#endif
  if (!ptr) throw std::bad_alloc();
  return ptr;
}


/* (22) = (b) ; AAN */
void * EF_CDECL operator new[]( EF_SIZE_T size, const std::nothrow_t & )
throw()
{
#ifndef EF_NO_LEAKDETECTION
  void *ptr = _eff_malloc(size, EFST_ALLOC_NEW_ARRAY, unknown_cxx_file, 0);
#else
  void *ptr = _eff_malloc(size);
#endif
  return ptr;
}


/* (23) = (c) ; FAW */
void   EF_CDECL operator delete[]( void * ptr )
throw()
{
#ifndef EF_NO_LEAKDETECTION
  _eff_free(ptr, EFST_ALLOC_NEW_ARRAY);
#else
  _eff_free(ptr);
#endif
}


/* (24) = (d) ; FAN */
void   EF_CDECL operator delete[]( void * ptr, const std::nothrow_t & )
throw()
{
#ifndef EF_NO_LEAKDETECTION
  _eff_free(ptr, EFST_ALLOC_NEW_ARRAY);
#else
  _eff_free(ptr);
#endif
}



/********************************************************************
********************************************************************/

#ifndef EF_NO_LEAKDETECTION

/* 3x : SINGLE OBJECT FORM - WITH DEBUG INFORMATION */


/* (31) = (a) ; ASW */
void * EF_CDECL operator new( EF_SIZE_T size, const char *filename, int lineno )
throw( std::bad_alloc )
{
  void *ptr = _eff_malloc(size, EFST_ALLOC_NEW_ELEM, filename, lineno);
  if (!ptr) throw std::bad_alloc();
  return ptr;
}

/* (32) = (b) ; ASN */
void * EF_CDECL operator new( EF_SIZE_T size, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  void *ptr = _eff_malloc(size, EFST_ALLOC_NEW_ELEM, filename, lineno);
  return ptr;
}


/* (33) = (c) ; FSW */
void   EF_CDECL operator delete( void *ptr, const char *filename, int lineno )
throw()
{
  _eff_free(ptr, EFST_ALLOC_NEW_ELEM);
}


/* (34) = (d) ; FSN */
void   EF_CDECL operator delete( void *ptr, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  _eff_free(ptr, EFST_ALLOC_NEW_ELEM);
}


/********************************************************************
********************************************************************/

/* 4x : ARRAY OBJECT FORM - WITH DEBUG INFORMATION */


/* (41) = (a) ; AAW */
void * EF_CDECL operator new[]( EF_SIZE_T size, const char *filename, int lineno )
throw( std::bad_alloc )
{
  void *ptr = _eff_malloc(size, EFST_ALLOC_NEW_ARRAY, filename, lineno);
  if (!ptr) throw std::bad_alloc();
  return ptr;
}


/* (42) = (b) ; AAN */
void * EF_CDECL operator new[]( EF_SIZE_T size, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  void *ptr = _eff_malloc(size, EFST_ALLOC_NEW_ARRAY, filename, lineno);
  return ptr;
}


/* (43) = (c) ; FAW */
void   EF_CDECL operator delete[]( void *ptr, const char *filename, int lineno )
throw()
{
  _eff_free(ptr, EFST_ALLOC_NEW_ARRAY);
}


/* (44) = (d) ; FAN */
void   EF_CDECL operator delete[]( void *ptr, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  _eff_free(ptr, EFST_ALLOC_NEW_ARRAY);
}

#endif /* end ifdef EF_NO_LEAKDETECTION */

#endif /* EF_NO_CPP_SUPPORT */
