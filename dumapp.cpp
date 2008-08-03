
/*
 * DUMA - Red-Zone memory allocator.
 * Copyright (C) 2002-2008 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
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

#ifdef _MSC_VER
#include <new.h>
#include <new>
#else
#include <new>
#endif

#include <stdlib.h>

#include "dumapp.h"
#include "noduma.h"

/* define the global variables */

static const char unknown_cxx_file[] =
 "UNKNOWN (use #include \"dumapp.h\")";

#ifndef DUMA_NO_LEAKDETECTION
/* with Leak-Detection */
#define DUMA_PARAMS_UK          , unknown_cxx_file, 0
#define DUMA_PARAMS_FILELINE    , filename, lineno
#else
/* no Leak-Detection */
#define DUMA_PARAMS_UK
#define DUMA_PARAMS_FILELINE
#endif

/********************************************************************
********************************************************************/

#ifdef _MSC_VER

int DUMA_CDECL duma_new_handler( size_t size )  { return 0; }

#else

void duma_new_handler() { }

#endif


// declare function ; needed for attribute
static
#ifdef _MSC_VER
__forceinline
#else
inline
#endif
void * duma_new_operator(DUMA_SIZE_T userSize, enum _DUMA_Allocator allocator, bool dothrow
#ifndef DUMA_NO_LEAKDETECTION
                         , const char * filename, int lineno
#endif
                        )
#ifdef __GNUC__
 __attribute__ ((always_inline))
#endif
;

/* Function: duma_new_operator
 *
 * C++ allocation function,
 * which is nearly C++ conform.
 *
 * Missing features are:
 *
 * - check if new_handler function hangs, return NULL or throw bad_alloc() in this case
 *    on UNIX systems alarm() may be usable
 *
 * Following features are implemented:
 * 
 * - loop is done
 * - alignment depends on user
 * - bad_alloc() exception will be thrown
 * - bad_alloc() is catched to return (void*)0
 * - check for userSize == 0, return non-NULL in this case
 *   implemented in new/delete operators below
 *
 */
void * duma_new_operator(DUMA_SIZE_T userSize, enum _DUMA_Allocator allocator, bool dothrow
#ifndef DUMA_NO_LEAKDETECTION
                         , const char * filename, int lineno
#endif
                        )
{
  void * ret = 0;
#ifdef _MSC_VER
  int     pnhret = 1;
#else
  #define pnhret 1
#endif

  // initialize duma?
  if ( _duma_g.allocList == 0 )
    _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */

  DUMA_TLSVARS_T  * duma_tls = GET_DUMA_TLSVARS();

  do
  {
    // try allocation
    ret = _duma_allocate( 0 /*=alignment*/
                        , userSize
                        , duma_tls->PROTECT_BELOW
                        , duma_tls->FILL
                        , 1 /*=protectAllocList*/
                        , allocator
                        , DUMA_FAIL_NULL
    #ifndef DUMA_NO_LEAKDETECTION
                        , filename
                        , lineno
    #endif
                        );

    // when allocation failed (and last call to new_handler didn't fail on Visual C++)
    if ( !ret && pnhret )
    {
      // get the current new_handler
    #ifdef _MSC_VER
      _PNH h = _set_new_handler(duma_new_handler);
      _set_new_handler(h);
    #else
      std::new_handler h = std::set_new_handler(duma_new_handler);
      std::set_new_handler(h);
    #endif

      // is there any handler?
      if ( h )
      {
        try               // new_handler may throw an exception!!!
        {
          // call handler
        #ifdef _MSC_VER
          pnhret = h(userSize);
        #else
          h();
        #endif
        }
        catch (std::bad_alloc)        // error occured in new_handler
        {
          if (dothrow)  throw;        // report error via re-throwing
          else          return ret;   // report error via (void*)0
        } // end try catch
      }
      else if (dothrow)   // report non-existing handler via exception
        throw std::bad_alloc();
      else                // report non-existing handler via (void*)0
        return ret;
    }
  } while ( !ret );       // loop until memory is claimed
  return ret;
}



/********************************************************************
********************************************************************/

/* Function: operator new
 *
 * Single object form, no debug information
 * (11) = (a) ; ASW
 */
void * DUMA_CDECL operator new( DUMA_SIZE_T size )
throw(std::bad_alloc)
{
  return duma_new_operator(size, EFA_NEW_ELEM, true  DUMA_PARAMS_UK);
}


/* Function: operator new
 *
 * Single object form, no debug information
 * (12) = (b) ; ASN
 */
void * DUMA_CDECL operator new( DUMA_SIZE_T size, const std::nothrow_t & )
throw()
{
  return duma_new_operator(size, EFA_NEW_ELEM, false  DUMA_PARAMS_UK);
}


/* Function: operator new
 *
 * Single object form, no debug information
 * (13) = ( c ) ; FSW
 */
void DUMA_CDECL operator delete( void *ptr )
throw()
{
#ifndef DUMA_NO_LEAKDETECTION
  DUMA_TLSVARS_T & duma_tls = * GET_DUMA_TLSVARS();
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM, duma_tls.DelFile[duma_tls.DelPtr], duma_tls.DelLine[duma_tls.DelPtr]);
#else
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM  DUMA_PARAMS_UK);
#endif
}


/* Function: operator new
 *
 * Single object form, no debug information
 * (14) = (d) ; FSN
 */
void   DUMA_CDECL operator delete( void * ptr, const std::nothrow_t & )
throw()
{
#ifndef DUMA_NO_LEAKDETECTION
  DUMA_TLSVARS_T & duma_tls = * GET_DUMA_TLSVARS();
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM, duma_tls.DelFile[duma_tls.DelPtr], duma_tls.DelLine[duma_tls.DelPtr]);
#else
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM  DUMA_PARAMS_UK);
#endif
}



/********************************************************************
********************************************************************/

/* 2x : ARRAY OBJECT FORM - NO DEBUG INFORMATION */

/* Function: operator new[]
 *
 * Array object form, no debug information
 * (21) = (a) ; AAW
 */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size )
throw(std::bad_alloc)
{
  return duma_new_operator(size, EFA_NEW_ARRAY, true  DUMA_PARAMS_UK);
}


/* Function: operator new[]
 *
 * Array object form, no debug information
 * (22) = (b) ; AAN 
 */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size, const std::nothrow_t & )
throw()
{
  return duma_new_operator(size, EFA_NEW_ARRAY, false  DUMA_PARAMS_UK);
}


/* Function: operator new[]
 *
 * Array object form, no debug information
 * (23) = (c) ; FAW 
 */
void   DUMA_CDECL operator delete[]( void * ptr )
throw()
{
#ifndef DUMA_NO_LEAKDETECTION
  DUMA_TLSVARS_T & duma_tls = * GET_DUMA_TLSVARS();
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY, duma_tls.DelFile[duma_tls.DelPtr], duma_tls.DelLine[duma_tls.DelPtr]);
#else
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY  DUMA_PARAMS_UK);
#endif
}


/* Function: operator new[]
 *
 * Array object form, no debug information
 * (24) = (d) ; FAN
 */
void   DUMA_CDECL operator delete[]( void * ptr, const std::nothrow_t & )
throw()
{
#ifndef DUMA_NO_LEAKDETECTION
  DUMA_TLSVARS_T & duma_tls = * GET_DUMA_TLSVARS();
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY, duma_tls.DelFile[duma_tls.DelPtr], duma_tls.DelLine[duma_tls.DelPtr]);
#else
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY  DUMA_PARAMS_UK);
#endif
}



/********************************************************************
********************************************************************/

#ifndef DUMA_NO_LEAKDETECTION

/* 3x : SINGLE OBJECT FORM - WITH DEBUG INFORMATION */


/* Function: operator new
 *
 * Single object form with debug information
 * (31) = (a) ; ASW 
 */
void * DUMA_CDECL operator new( DUMA_SIZE_T size, const char *filename, int lineno )
throw( std::bad_alloc )
{
  return duma_new_operator(size, EFA_NEW_ELEM, true  DUMA_PARAMS_FILELINE);
}

/* Function: operator new
 *
 * Single object form with debug information
 * (32) = (b) ; ASN 
 */
void * DUMA_CDECL operator new( DUMA_SIZE_T size, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  return duma_new_operator(size, EFA_NEW_ELEM, false  DUMA_PARAMS_FILELINE);
}


/* Function: operator new
 *
 * Single object form with debug information
 * (33) = (c) ; FSW 
 */
void   DUMA_CDECL operator delete( void *ptr, const char *filename, int lineno )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM  DUMA_PARAMS_FILELINE);
}


/* Function: operator new
 *
 * Single object form with debug information
 * (34) = (d) ; FSN 
 */
void   DUMA_CDECL operator delete( void *ptr, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ELEM  DUMA_PARAMS_FILELINE);
}


/********************************************************************
********************************************************************/

/* 4x : ARRAY OBJECT FORM - WITH DEBUG INFORMATION */


/* Function: operator new[]
 *
 * Array object form with debug information
 * (41) = (a) ; AAW 
 */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size, const char *filename, int lineno )
throw( std::bad_alloc )
{
  return duma_new_operator(size, EFA_NEW_ARRAY, true  DUMA_PARAMS_FILELINE);
}


/* Function: operator new[]
 *
 * Array object form with debug information
 * (42) = (b) ; AAN 
 */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  return duma_new_operator(size, EFA_NEW_ARRAY, false  DUMA_PARAMS_FILELINE);
}


/* Function: operator new[]
 *
 * Array object form with debug information
 * (43) = (c) ; FAW 
 */
void   DUMA_CDECL operator delete[]( void *ptr, const char *filename, int lineno )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY  DUMA_PARAMS_FILELINE);
}


/* Function: operator new[]
 *
 * Array object form with debug information
 * (44) = (d) ; FAN 
 */
void   DUMA_CDECL operator delete[]( void *ptr, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  _duma_deallocate(ptr, 1 /*=protectAllocList*/, EFA_DEL_ARRAY  DUMA_PARAMS_FILELINE);
}

#endif /* end ifdef DUMA_NO_LEAKDETECTION */

#endif /* DUMA_NO_CPP_SUPPORT */

