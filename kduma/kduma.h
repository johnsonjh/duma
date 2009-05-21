
/* 
 * KDUMA - Kernel Mode Red-Zone memory allocator.
 * Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
 * Copyright (C) 2006 Eric Rachner <eric@rachner.us>
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
 */

/* $Id$ */

/*
 * #include <stdlib.h>
 *
 * You must include <stdlib.h> before including <duma.h> !
 *
 */

/* for enabling inclusion of duma.h after inclusion of efencint.h */
/* remove previous definitions */
#include "noduma.h"

#ifndef __KDUMA_H__
#define __KDUMA_H__

#include "duma_config.h"

#ifdef __cplusplus
	#define DUMA_EXTERN_C   extern "C"
#else
	#define DUMA_EXTERN_C   extern
#endif

/* global DUMA variables */
DUMA_EXTERN_C int DUMA_OUTPUT_DEBUG;
DUMA_EXTERN_C int DUMA_OUTPUT_STDOUT;
DUMA_EXTERN_C int DUMA_OUTPUT_STDERR;
DUMA_EXTERN_C char* DUMA_OUTPUT_FILE;
DUMA_EXTERN_C int DUMA_OUTPUT_STACKTRACE;
DUMA_EXTERN_C int  DUMA_PROTECT_BELOW;
DUMA_EXTERN_C size_t  DUMA_ALIGNMENT;
DUMA_EXTERN_C int  DUMA_FILL;
DUMA_EXTERN_C struct _DUMA_Slot * _duma_allocList;
#ifndef DUMA_NO_CPP_SUPPORT
DUMA_EXTERN_C void * _duma_cxx_null_addr;
#endif
#endif /* DUMA_EXTERNS_DECLARED */

/* Enum: _DUMA_Allocator
 *
 * Passed to duma allocator such as we know who
 * is calling us.
 */
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

/* Enum: _DUMA_FailReturn
 *
 * Unknown use.
 */
enum _DUMA_FailReturn
{
	DUMA_FAIL_NULL
	, DUMA_FAIL_ENV
};

DUMA_EXTERN_C void _duma_init(void);
DUMA_EXTERN_C void _duma_assert(const char * exprstr, const char * filename, int lineno);

DUMA_EXTERN_C void   duma_init(void);

DUMA_EXTERN_C void * _duma_allocate(size_t alignment, size_t userSize, int protectBelow, int fillByte, int protectAllocList, enum _DUMA_Allocator allocator, enum _DUMA_FailReturn fail, const char * filename, int lineno);
DUMA_EXTERN_C void   _duma_deallocate(void * baseAdr, int protectAllocList, enum _DUMA_Allocator allocator, const char * filename, int lineno);

DUMA_EXTERN_C void * _duma_kalloc(size_t size, int flags, const char * filename, int lineno);
DUMA_EXTERN_C void   _duma_kfree(void *baseAdr, const char * filename, int lineno);

DUMA_EXTERN_C void * _duma_valloc(size_t size, const char * filename, int lineno);
DUMA_EXTERN_C void * _duma_vfree(void *baseAdr, const char * filename, int lineno);

DUMA_EXTERN_C char * _duma_strdup(const char *str, const char * filename, int lineno);
DUMA_EXTERN_C void * _duma_memcpy(void *dest, const void *src, size_t size, const char * filename, int lineno);
DUMA_EXTERN_C void * _duma_memmove(void *dest, const void *src, size_t size);
DUMA_EXTERN_C char * _duma_strcpy(char *dest, const char *src, const char * filename, int lineno);
DUMA_EXTERN_C char * _duma_strncpy(char *dest, const char *src, size_t size, const char * filename, int lineno);
DUMA_EXTERN_C char * _duma_strcat(char *dest, const char *src, const char * filename, int lineno);
DUMA_EXTERN_C char * _duma_strncat(char *dest, const char *src, size_t size, const char * filename, int lineno);

DUMA_EXTERN_C void  DUMA_newFrame(void);
DUMA_EXTERN_C void  DUMA_delFrame(void);


#define kalloc(SIZE, FLAGS)			_duma_kalloc(SIZE, FLAGS, __FILE__, __LINE__)
#define kfree(BASEADR)				_duma_kfree(BASEADR, __FILE__, __LINE__)
#define valloc(SIZE)				_duma_valloc(SIZE, __FILE__, __LINE__)
#define vfree(BASEADR)				_duma_vfree(BASEADR, __FILE__, __LINE__)

#define strdup(STR)					_duma_strdup(STR, __FILE__, __LINE__)
#define memcpy(DEST, SRC, SIZE)		_duma_memcpy(DEST, SRC, SIZE, __FILE__, __LINE__)
#define memmove(DEST, SRC, SIZE)		_duma_memmove(DEST, SRC, SIZE)
#define strcpy(DEST, SRC)			_duma_strcpy(DEST, SRC, __FILE__, __LINE__)
#define strncpy(DEST, SRC, SIZE)	_duma_strncpy(DEST, SRC, SIZE, __FILE__, __LINE__)
#define strcat(DEST, SRC)			_duma_strcat(DEST, SRC, __FILE__, __LINE__)
#define strncat(DEST, SRC, SIZE)	_duma_strncat(DEST, SRC, SIZE, __FILE__, __LINE__)

#ifndef DUMA_ASSERT
	#define DUMA_ASSERT(EXPR)	(  (EXPR) || ( _duma_assert(#EXPR, __FILE__, __LINE__), 0 )  )
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


#endif /* end ifdef __KDUMA_H__ */

// end
