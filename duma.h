
/*
 * DUMA - Red-Zone memory allocator.
 * Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
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
 * header file for inclusion from YOUR application code
 */

#include <stdlib.h>

/*
 * #include <stdlib.h>
 *
 * You must include <stdlib.h> before including <duma.h> !
 *
 */

/* for enabling inclusion of duma.h after inclusion of efencint.h */
/* remove previous definitions */
#include "noduma.h"

#include "duma_config.h"

#ifdef __cplusplus
	#define DUMA_EXTERN_C   extern "C"
#else
	#define DUMA_EXTERN_C   extern
#endif

#ifdef DUMA_NO_DUMA

	/* enable these macros even in release code, but do nothing */
	#define DUMA_newFrame()             do { } while(0)
	#define DUMA_delFrame()             do { } while(0)

	#define DUMA_ASSERT(EXPR)           do { } while(0)

	#define CA_DECLARE(NAME,SIZE)       do { } while(0)
	#define CA_DEFINE(TYPE,NAME,SIZE)   TYPE NAME[SIZE]
	#define CA_REF(NAME,INDEX)          NAME[INDEX]

#else /* ifndef DUMA_NO_DUMA */

	#ifndef DUMA_EXTERNS_DECLARED
	#define DUMA_EXTERNS_DECLARED
	/* global DUMA variables */
	DUMA_EXTERN_C int DUMA_OUTPUT_DEBUG;
	DUMA_EXTERN_C int DUMA_OUTPUT_STDOUT;
	DUMA_EXTERN_C int DUMA_OUTPUT_STDERR;
	DUMA_EXTERN_C char* DUMA_OUTPUT_FILE;
	DUMA_EXTERN_C int DUMA_OUTPUT_STACKTRACE;
	DUMA_EXTERN_C int  DUMA_PROTECT_BELOW;
	DUMA_EXTERN_C int  DUMA_ALIGNMENT;
	DUMA_EXTERN_C int  DUMA_FILL;
	DUMA_EXTERN_C struct _DUMA_Slot * _duma_allocList;
	#ifndef DUMA_NO_CPP_SUPPORT
	DUMA_EXTERN_C void * _duma_cxx_null_addr;
	#endif
	#endif /* DUMA_EXTERNS_DECLARED */

	#ifndef DUMA_ENUMS_DECLARED
	#define DUMA_ENUMS_DECLARED

	enum _DUMA_Allocator
	{
		EFA_INT_ALLOC
	, EFA_INT_DEALLOC
	, EFA_MALLOC
	, EFA_CALLOC
	, EFA_FREE
	, EFA_MEMALIGN
	, EFA_REALLOC
	, EFA_VALLOC
	, EFA_STRDUP
	, EFA_NEW_ELEM
	, EFA_DEL_ELEM
	, EFA_NEW_ARRAY
	, EFA_DEL_ARRAY

	/* use following enums when calling _duma_allocate()/_duma_deallocate()
	* from user defined member operators
	*/
	, EFA_MEMBER_NEW_ELEM
	, EFA_MEMBER_DEL_ELEM
	, EFA_MEMBER_NEW_ARRAY
	, EFA_MEMBER_DEL_ARRAY
	};

	enum _DUMA_FailReturn
	{
		DUMA_FAIL_NULL
	, DUMA_FAIL_ENV
	};

#endif /* DUMA_ENUMS_DECLARED */


#ifndef DUMA_FUNCTIONS_DECLARED
#define DUMA_FUNCTIONS_DECLARED

  DUMA_EXTERN_C void _duma_init(void);
  DUMA_EXTERN_C void _duma_assert(const char * exprstr, const char * filename, int lineno);

#ifdef DUMA_EXPLICIT_INIT
DUMA_EXTERN_C void   duma_init(void);
#endif

  #ifndef DUMA_NO_LEAKDETECTION
    DUMA_EXTERN_C void * _duma_allocate(size_t alignment, size_t userSize, int protectBelow, int fillByte, int protectAllocList, enum _DUMA_Allocator allocator, enum _DUMA_FailReturn fail, const char * filename, int lineno);
    DUMA_EXTERN_C void   _duma_deallocate(void * baseAdr, int protectAllocList, enum _DUMA_Allocator allocator, const char * filename, int lineno);
    DUMA_EXTERN_C void * _duma_malloc(size_t size, const char * filename, int lineno);
    DUMA_EXTERN_C void * _duma_calloc(size_t elemCount, size_t elemSize, const char * filename, int lineno);
    DUMA_EXTERN_C void   _duma_free(void * baseAdr, const char * filename, int lineno);
    DUMA_EXTERN_C void * _duma_memalign(size_t alignment, size_t userSize, const char * filename, int lineno);
    DUMA_EXTERN_C void * _duma_realloc(void * baseAdr, size_t newSize, const char * filename, int lineno);
    DUMA_EXTERN_C void * _duma_valloc(size_t size, const char * filename, int lineno);
    DUMA_EXTERN_C char * _duma_strdup(const char *str, const char * filename, int lineno);
    DUMA_EXTERN_C void * _duma_memcpy(void *dest, const void *src, size_t size, const char * filename, int lineno);
    DUMA_EXTERN_C char * _duma_strcpy(char *dest, const char *src, const char * filename, int lineno);
    DUMA_EXTERN_C char * _duma_strncpy(char *dest, const char *src, size_t size, const char * filename, int lineno);
    DUMA_EXTERN_C char * _duma_strcat(char *dest, const char *src, const char * filename, int lineno);
    DUMA_EXTERN_C char * _duma_strncat(char *dest, const char *src, size_t size, const char * filename, int lineno);
    DUMA_EXTERN_C void  DUMA_newFrame(void);
    DUMA_EXTERN_C void  DUMA_delFrame(void);
  #else /* DUMA_NO_LEAKDETECTION */
    DUMA_EXTERN_C void * _duma_allocate(size_t alignment, size_t userSize, int protectBelow, int fillByte, int protectAllocList, enum _DUMA_Allocator allocator, enum _DUMA_FailReturn fail);
    DUMA_EXTERN_C void   _duma_deallocate(void * baseAdr, int protectAllocList, enum _DUMA_Allocator allocator);
    DUMA_EXTERN_C void * _duma_malloc(size_t size);
    DUMA_EXTERN_C void * _duma_calloc(size_t elemCount, size_t elemSize);
    DUMA_EXTERN_C void   _duma_free(void * baseAdr);
    DUMA_EXTERN_C void * _duma_memalign(size_t alignment, size_t userSize);
    DUMA_EXTERN_C void * _duma_realloc(void * baseAdr, size_t newSize);
    DUMA_EXTERN_C void * _duma_valloc(size_t size);
    DUMA_EXTERN_C char * _duma_strdup(const char *str);
    DUMA_EXTERN_C void * _duma_memcpy(void *dest, const void *src, size_t size);
    DUMA_EXTERN_C char * _duma_strcpy(char *dest, const char *src);
    DUMA_EXTERN_C char * _duma_strncpy(char *dest, const char *src, size_t size);
    DUMA_EXTERN_C char * _duma_strcat(char *dest, const char *src);
    DUMA_EXTERN_C char * _duma_strncat(char *dest, const char *src, size_t size);
  #endif /* DUMA_NO_LEAKDETECTION */

#endif /* DUMA_FUNCTIONS_DECLARED */

#ifndef DUMA_SKIP_SETUP
#ifndef DUMA_NO_LEAKDETECTION
  #define malloc(SIZE)                _duma_malloc(SIZE, __FILE__, __LINE__)
  #define calloc(ELEMCOUNT, ELEMSIZE) _duma_calloc(ELEMCOUNT, ELEMSIZE, __FILE__, __LINE__)
  #define free(BASEADR)               _duma_free(BASEADR, __FILE__, __LINE__)
  #define memalign(ALIGNMENT, SIZE)   _duma_memalign(ALIGNMENT, SIZE, __FILE__, __LINE__)
  #define realloc(BASEADR, NEWSIZE)   _duma_realloc(BASEADR, NEWSIZE, __FILE__, __LINE__)
  #define valloc(SIZE)                _duma_valloc(SIZE, __FILE__, __LINE__)
  #define strdup(STR)                 _duma_strdup(STR, __FILE__, __LINE__)
  #define memcpy(DEST, SRC, SIZE)     _duma_memcpy(DEST, SRC, SIZE, __FILE__, __LINE__)
  #define strcpy(DEST, SRC)           _duma_strcpy(DEST, SRC, __FILE__, __LINE__)
  #define strncpy(DEST, SRC, SIZE)    _duma_strncpy(DEST, SRC, SIZE, __FILE__, __LINE__)
  #define strcat(DEST, SRC)           _duma_strcat(DEST, SRC, __FILE__, __LINE__)
  #define strncat(DEST, SRC, SIZE)    _duma_strncat(DEST, SRC, SIZE, __FILE__, __LINE__)
#else /* DUMA_NO_LEAKDETECTION */
  #define DUMA_newFrame()             do { } while(0)
  #define DUMA_delFrame()             do { } while(0)
#endif /* DUMA_NO_LEAKDETECTION */
#endif // DUMA_SKIP_SETUP

#ifndef DUMA_ASSERT
  #define DUMA_ASSERT(EXPR)    (  (EXPR) || ( _duma_assert(#EXPR, __FILE__, __LINE__), 0 )  )
#endif


/*
 * protection of functions return address
 */
#ifdef __GNUC__

#define DUMA_FN_PROT_START      const void * DUMA_RET_ADDR = __builtin_return_address(0); {

#define DUMA_FN_PROT_END        } DUMA_ASSERT( __builtin_return_address(0) == DUMA_RET_ADDR );

#define DUMA_FN_PROT_RET(EXPR)  do {  DUMA_ASSERT( __builtin_return_address(0) == DUMA_RET_ADDR );  return( EXPR ); }  while (0)
#define DUMA_FN_PROT_RET_VOID() do {  DUMA_ASSERT( __builtin_return_address(0) == DUMA_RET_ADDR );  return;         }  while (0)

#else

#define DUMA_FN_PROT_START      int aiDUMA_PROT[ 4 ] = { 'E', 'F', 'P', 'R' }; {

#define DUMA_FN_PROT_END        } DUMA_ASSERT( 'E'==aiDUMA_PROT[0] && 'F'==aiDUMA_PROT[1] && 'P'==aiDUMA_PROT[2] && 'R'==aiDUMA_PROT[3] );

#define DUMA_FN_PROT_RET(EXPR)  do {  DUMA_ASSERT( 'E'==aiDUMA_PROT[0] && 'F'==aiDUMA_PROT[1] && 'P'==aiDUMA_PROT[2] && 'R'==aiDUMA_PROT[3] );  return( EXPR ); }  while (0)
#define DUMA_FN_PROT_RET_VOID() do {  DUMA_ASSERT( 'E'==aiDUMA_PROT[0] && 'F'==aiDUMA_PROT[1] && 'P'==aiDUMA_PROT[2] && 'R'==aiDUMA_PROT[3] );  return;         }  while (0)

#endif

/* declaration of an already defined array to enable checking at every reference
 * when using CA_REF()
 */
#define CA_DECLARE(NAME,SIZE) \
const unsigned long NAME ## _checkedsize = (SIZE); \
unsigned long NAME ## _checkedidx

/* definition of a checked array adds definitions for its size and an extra temporary.
 * every array gets its own temporary to avoid problems with threading
 * a global temporary would have.
 */
#define CA_DEFINE(TYPE,NAME,SIZE)  TYPE NAME[SIZE]; CA_DECLARE(NAME,SIZE)

/* every access to a checked array is preceded an assert() on the index;
 * the index parameter is stored to a temporary to avoid double execution of index,
 * when index contains f.e. a "++".
 */
#define CA_REF(NAME,INDEX) \
NAME[ DUMA_ASSERT( (NAME ## _checkedidx = (INDEX)) < NAME ## _checkedsize ), NAME ## _checkedidx ]


#endif /* end ifdef DUMA_NO_DUMA */
