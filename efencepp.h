
/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 * License: GNU LGPL (GNU Lesser General Public License, see COPYING-GPL)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * FILE CONTENTS:
 * header file for inclusion from YOUR C++ application code
 * various new/delete/new[]/delete[] declarations
 * you must include <new>  before including this file.
 */

#ifndef _EFENCEPP_H_
#define _EFENCEPP_H_

#ifndef	__cplusplus
/* avoid usage of C++ operator replacements in C code */
#error compile with a C++ compiler, or define EF_NO_CPP_SUPPORT to remove this error
#endif

#include "efence.h"

/* remove previous macro definitions */
#include "noefence.h"


#if ( defined(EF_NO_CPP_SUPPORT) || defined(EF_NO_LEAKDETECTION) )

/* define macros as wrapper without special functionality */
#define NEW_ELEM(TYPE)                  new TYPE
#define NEW_ARRAY(TYPE, COUNT)          new TYPE[COUNT]
#define DEL_ELEM(PTR)                   delete PTR
#define DEL_ARRAY(PTR)                  delete []PTR

#endif /* ( defined(EF_NO_CPP_SUPPORT) || defined(EF_NO_LEAKDETECTION) ) */


#ifndef EF_NO_CPP_SUPPORT

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

/* (1) - (4) each throw a std::bad_alloc exception */

/*
 * Classification
 * A allocation          <-> F free
 * S single object form  <-> A array form
 * W without nothrow     <-> N with std::nothrow_t parameter
*/

/* 1x : SINGLE OBJECT FORM - NO DEBUG INFORMATION */
/* (11) = (a) ; ASW */
/* (12) = (b) ; ASN */
/* (13) = (c) ; FSW */
/* (14) = (d) ; FSN */
void * EF_CDECL operator new( EF_SIZE_T )                             throw(std::bad_alloc);
void * EF_CDECL operator new( EF_SIZE_T , const std::nothrow_t & )    throw();
void   EF_CDECL operator delete( void * )                             throw();
void   EF_CDECL operator delete( void * , const std::nothrow_t & )    throw();

/* 2x : ARRAY OBJECT FORM - NO DEBUG INFORMATION */
/* (21) = (a) ; AAW */
/* (22) = (b) ; AAN */
/* (23) = (c) ; FAW */
/* (24) = (d) ; FAN */
void * EF_CDECL operator new[]( EF_SIZE_T )                           throw(std::bad_alloc);
void * EF_CDECL operator new[]( EF_SIZE_T , const std::nothrow_t & )  throw();
void   EF_CDECL operator delete[]( void * )                           throw();
void   EF_CDECL operator delete[]( void *, const std::nothrow_t & )   throw();

#ifndef EF_NO_LEAKDETECTION

/* 3x : SINGLE OBJECT FORM - WITH DEBUG INFORMATION */
/* (31) = (a) ; ASW */
/* (32) = (b) ; ASN */
/* (33) = (c) ; FSW */
/* (34) = (d) ; FSN */
void * EF_CDECL operator new( EF_SIZE_T, const char *, int )                         throw( std::bad_alloc );
void * EF_CDECL operator new( EF_SIZE_T, const std::nothrow_t &, const char *, int ) throw();
void   EF_CDECL operator delete( void *, const char *, int )                         throw();
void   EF_CDECL operator delete( void *, const std::nothrow_t &, const char *, int ) throw();

/* 4x : ARRAY OBJECT FORM - WITH DEBUG INFORMATION */
/* (41) = (a) ; AAW */
/* (42) = (b) ; AAN */
/* (43) = (c) ; FAW */
/* (44) = (d) ; FAN */
void * EF_CDECL operator new[]( EF_SIZE_T, const char *, int )                         throw( std::bad_alloc );
void * EF_CDECL operator new[]( EF_SIZE_T, const std::nothrow_t &, const char *, int ) throw();
void   EF_CDECL operator delete[]( void *, const char *, int )                         throw();
void   EF_CDECL operator delete[]( void *, const std::nothrow_t &, const char *, int ) throw();

/* define macros as wrapper for our special operators */

#ifdef EF_OLD_NEW_MACRO
  #define NEW_ELEM(TYPE)          new(__FILE__,__LINE__) TYPE
  #define NEW_ARRAY(TYPE, COUNT)  new(__FILE__,__LINE__) TYPE[COUNT]
#else
  #define new                     new(__FILE__, __LINE__)
#endif

#define DEL_ELEM(PTR)             operator delete  (PTR, __FILE__, __LINE__)
#define DEL_ARRAY(PTR)            operator delete[](PTR, __FILE__, __LINE__)

#endif /* end ifdef EF_NO_LEAKDETECTION */

#endif /*	end ifdef EF_NO_CPP_SUPPORT */

#endif /* _EFENCEPP_H_ */

