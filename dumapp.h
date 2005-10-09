
/*
 * DUMA - Red-Zone memory allocator.
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

#ifndef	__cplusplus
/* avoid usage of C++ operator replacements in C code */
#error compile with a C++ compiler, or define DUMA_NO_CPP_SUPPORT to remove this error
#endif

#include "duma.h"

/* remove previous macro definitions */
#include "noduma.h"


#if ( defined(DUMA_NO_CPP_SUPPORT) || defined(DUMA_NO_LEAKDETECTION) )
  /* define macros as wrapper without special functionality */
  #define NEW_ELEM(TYPE)                  new TYPE
  #define NEW_ARRAY(TYPE, COUNT)          new TYPE[COUNT]
  #define DEL_ELEM(PTR)                   delete PTR
  #define DEL_ARRAY(PTR)                  delete []PTR
#endif /* ( defined(DUMA_NO_CPP_SUPPORT) || defined(DUMA_NO_LEAKDETECTION) ) */


#ifndef DUMA_CDECL
  #ifdef _MSC_VER
    #define DUMA_CDECL  __cdecl
  #else
    #define DUMA_CDECL
  #endif
#endif

#ifndef DUMA_SIZE_T
  #ifdef _MSC_VER
    #define DUMA_SIZE_T    size_t
  #else
    #define DUMA_SIZE_T    std::size_t
  #endif
#endif


#ifndef DUMA_NO_CPP_SUPPORT

  #ifndef DUMA_CPP_OPERATORS_DECLARED /* && !defined(DUMA_NO_CPP_SUPPORT) */
  #define DUMA_CPP_OPERATORS_DECLARED
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
    void * DUMA_CDECL operator new(DUMA_SIZE_T) throw(std::bad_alloc);
    void * DUMA_CDECL operator new(DUMA_SIZE_T, const std::nothrow_t &) throw();
    void   DUMA_CDECL operator delete(void *) throw();
    void   DUMA_CDECL operator delete(void *, const std::nothrow_t &)   throw();

    /* 2x : ARRAY OBJECT FORM - NO DEBUG INFORMATION */
    /* (21) = (a) ; AAW */
    /* (22) = (b) ; AAN */
    /* (23) = (c) ; FAW */
    /* (24) = (d) ; FAN */
    void * DUMA_CDECL operator new[](DUMA_SIZE_T) throw(std::bad_alloc);
    void * DUMA_CDECL operator new[](DUMA_SIZE_T, const std::nothrow_t &) throw();
    void   DUMA_CDECL operator delete[](void *) throw();
    void   DUMA_CDECL operator delete[](void *, const std::nothrow_t &)   throw();

    #ifndef DUMA_NO_LEAKDETECTION /* && !defined(DUMA_CPP_OPERATORS_DECLARED) */

      /* 3x : SINGLE OBJECT FORM - WITH DEBUG INFORMATION */
      /* (31) = (a) ; ASW */
      /* (32) = (b) ; ASN */
      /* (33) = (c) ; FSW */
      /* (34) = (d) ; FSN */
      void * DUMA_CDECL operator new(DUMA_SIZE_T, const char *, int) throw(std::bad_alloc);
      void * DUMA_CDECL operator new(DUMA_SIZE_T, const std::nothrow_t &, const char *, int) throw();
      void   DUMA_CDECL operator delete(void *, const char *, int) throw();
      void   DUMA_CDECL operator delete(void *, const std::nothrow_t &, const char *, int) throw();

      /* 4x : ARRAY OBJECT FORM - WITH DEBUG INFORMATION */
      /* (41) = (a) ; AAW */
      /* (42) = (b) ; AAN */
      /* (43) = (c) ; FAW */
      /* (44) = (d) ; FAN */
      void * DUMA_CDECL operator new[](DUMA_SIZE_T, const char *, int) throw(std::bad_alloc);
      void * DUMA_CDECL operator new[](DUMA_SIZE_T, const std::nothrow_t &, const char *, int) throw();
      void   DUMA_CDECL operator delete[](void *, const char *, int) throw();
      void   DUMA_CDECL operator delete[](void *, const std::nothrow_t &, const char *, int) throw();

    #endif /* DUMA_NO_LEAKDETECTION */
  #endif /* DUMA_CPP_OPERATORS_DECLARED */

  #ifndef DUMA_NO_LEAKDETECTION /* && !defined(DUMA_CPP_OPERATORS_DECLARED) */

    #ifndef DUMA_NO_THREAD_SAFETY
      #include "sem_inc.h"
    #endif

    #ifndef DUMA_DELETE_VARS
    #define DUMA_DELETE_VARS
      /* set Maximum Delete Depth (depth of recursive destructor calls) */
      #ifndef DUMA_MAX_DEL_DEPTH
        #define DUMA_MAX_DEL_DEPTH    256
      #endif

      /* TODO following variables should exist per thread ("thread-local") */
      extern int          DUMA_DeletePtr;
      extern int          DUMA_Magic[DUMA_MAX_DEL_DEPTH];
      extern const char * DUMA_DeleteFile[DUMA_MAX_DEL_DEPTH];
      extern int          DUMA_DeleteLine[DUMA_MAX_DEL_DEPTH];
    #endif

    /* define macros as wrapper for our special operators */

    #ifdef DUMA_OLD_NEW_MACRO
      /* throwing variants */
      #define NEW_ELEM(TYPE)                  new(__FILE__,__LINE__) TYPE
      #define NEW_ARRAY(TYPE, COUNT)          new(__FILE__,__LINE__) TYPE[COUNT]
      /* non throwing variant */
      #define NEW_ELEM_NOTHROW(TYPE)          new(std::nothrow,__FILE__,__LINE__) TYPE
      #define NEW_ARRAY_NOTHROW(TYPE, COUNT)  new(std::nothrow,__FILE__,__LINE__) TYPE[COUNT]
    #else
      /* #define new_NOTHROW                      new(std::nothrow,__FILE__,__LINE__) */
      #define new                             new(__FILE__, __LINE__)
    #endif /* DUMA_OLD_NEW_MACRO */

    #ifdef DUMA_OLD_DEL_MACRO
      /* always use DUMA_DeletePtr/DUMA_DeleteFile/DUMA_DeleteLine
       * to allow best possible filename/line reports when
       * non-DUMA deallocations are called from destructors
       */
      /* non-throwing */
      #define DEL_ELEM(PTR)                   for( DUMA_Magic[++DUMA_DeletePtr] = 1;  \
                                                   DUMA_Magic[DUMA_DeletePtr];        \
                                                   DUMA_Magic[DUMA_DeletePtr--] = 0   \
                                                 ) operator delete  (PTR, __FILE__, __LINE__)
      #define DEL_ARRAY(PTR)                  for( DUMA_Magic[++DUMA_DeletePtr] = 1;  \
                                                   DUMA_Magic[DUMA_DeletePtr];        \
                                                   DUMA_Magic[DUMA_DeletePtr--] = 0   \
                                                 ) operator delete[](PTR, __FILE__, __LINE__)
      /* explicitly non-throwing */
      #define DEL_ELEM_NOTHROW(PTR)           for( DUMA_Magic[++DUMA_DeletePtr] = 1;  \
                                                   DUMA_Magic[DUMA_DeletePtr];        \
                                                   DUMA_Magic[DUMA_DeletePtr--] = 0   \
                                                 ) operator delete  (PTR, std::nothrow,__FILE__, __LINE__)
      #define DEL_ARRAY_NOTHROW(PTR)          for( DUMA_Magic[++DUMA_DeletePtr] = 1;  \
                                                   DUMA_Magic[DUMA_DeletePtr];        \
                                                   DUMA_Magic[DUMA_DeletePtr--] = 0   \
                                                 ) operator delete[](PTR, std::nothrow,__FILE__, __LINE__)
    #else
      #ifndef DUMA_NO_THREAD_SAFETY
        /* define a thread safe delete */
        #define delete        for( DUMA_GET_SEMAPHORE(),                        \
                                   DUMA_Magic[++DUMA_DeletePtr] = 1,            \
                                   DUMA_DeleteFile[DUMA_DeletePtr] = __FILE__,  \
                                   DUMA_DeleteLine[DUMA_DeletePtr] = __LINE__;  \
                                   DUMA_Magic[DUMA_DeletePtr];                  \
                                   DUMA_Magic[DUMA_DeletePtr--] = 0,            \
                                   DUMA_RELEASE_SEMAPHORE()                     \
                                                 ) delete
      #else
        #define delete        for( DUMA_Magic[++DUMA_DeletePtr] = 1,            \
                                   DUMA_DeleteFile[DUMA_DeletePtr] = __FILE__,  \
                                   DUMA_DeleteLine[DUMA_DeletePtr] = __LINE__;  \
                                   DUMA_Magic[DUMA_DeletePtr];                  \
                                   DUMA_Magic[DUMA_DeletePtr--] = 0             \
                                 ) delete
      #endif /* DUMA_NO_THREAD_SAFETY */
    #endif /* DUMA_OLD_DEL_MACRO */

  #endif /* end ifdef DUMA_NO_LEAKDETECTION */

#endif /*	end ifdef DUMA_NO_CPP_SUPPORT */

