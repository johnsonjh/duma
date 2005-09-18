
/*
 * DUMA - Red-Zone memory allocator.
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
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

#include "duma.h"


#if WIN32
#define DUMA_CDECL     __cdecl
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

#ifdef _MSC_VER

int DUMA_CDECL duma_new_handler( size_t size )  { return 0; }

#else

void duma_new_handler() { }

#endif


/* C++ allocation function,
 * which is nearly C++ conform. missing features are:
 * 1- check for userSize == 0, return non-NULL in this case
 * 2- check if new_handler function hangs, return NULL or throw bad_alloc() in this case
 *    on UNIX systems alarm() may be usable
 *
 * following features are implemented:
 * 1- loop is done
 * 2- alignment depends on user
 * 3- bad_alloc() exception will be thrown
 * 4- bad_alloc() is catched to return (void*)0
 *
 */
static inline
void * duma_new_operator(size_t userSize, enum _DUMA_Allocator allocator, bool dothrow
#ifndef DUMA_NO_LEAKDETECTION
                         , const char * filename, int lineno
#endif
                        )
#ifdef __GNUC__
 __attribute__ ((always_inline))
#endif
{
  void * ret = 0;
#ifdef _MSC_VER
  int     pnhret = 1;
#else
  #define pnhret 1
#endif

  // initialize duma?
  if ( _duma_allocList == 0 )
    _duma_init();  /* This sets DUMA_ALIGNMENT, DUMA_PROTECT_BELOW, DUMA_FILL, ... */

  do
  {
    // try allocation
    ret = _duma_allocate( 0 /*=alignment*/
                        , userSize
                        , DUMA_PROTECT_BELOW
                        , DUMA_FILL
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


/* 1x : SINGLE OBJECT FORM - NO DEBUG INFORMATION */

/* (11) = (a) ; ASW */
void * DUMA_CDECL operator new( DUMA_SIZE_T size )
throw(std::bad_alloc)
{
  return duma_new_operator(size, EFA_NEW_ELEM, true  DUMA_PARAMS_UK);
}


/* (12) = (b) ; ASN */
void * DUMA_CDECL operator new( DUMA_SIZE_T size, const std::nothrow_t & )
throw()
{
  return duma_new_operator(size, EFA_NEW_ELEM, false  DUMA_PARAMS_UK);
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
  return duma_new_operator(size, EFA_NEW_ARRAY, true  DUMA_PARAMS_UK);
}


/* (22) = (b) ; AAN */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size, const std::nothrow_t & )
throw()
{
  return duma_new_operator(size, EFA_NEW_ARRAY, false  DUMA_PARAMS_UK);
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
  return duma_new_operator(size, EFA_NEW_ELEM, true, filename, lineno);
}

/* (32) = (b) ; ASN */
void * DUMA_CDECL operator new( DUMA_SIZE_T size, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  return duma_new_operator(size, EFA_NEW_ELEM, false, filename, lineno);
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
  return duma_new_operator(size, EFA_NEW_ARRAY, true, filename, lineno);
}


/* (42) = (b) ; AAN */
void * DUMA_CDECL operator new[]( DUMA_SIZE_T size, const std::nothrow_t &, const char *filename, int lineno )
throw()
{
  return duma_new_operator(size, EFA_NEW_ARRAY, false, filename, lineno);
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

