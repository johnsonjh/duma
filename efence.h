
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
 * header file for inclusion from YOUR application code
 */

/*
 * #include <stdlib.h>
 *
 * You must include <stdlib.h> before including <efence.h> !
 *
 */

/* for enabling inclusion of efence.h after inclusion of efencint.h */
/* remove previous definitions */
#include "noefence.h"

#ifndef _EFENCE_CONFIG_H_
#include "efence_config.h"
#endif


#ifdef EF_NO_EFENCE

/* enable these macros even in release code, but do nothing */
#define EF_newFrame()               do { } while(0)
#define EF_delFrame()               do { } while(0)

#define EF_ASSERT(EXPR)             do { } while(0)

#define CA_DECLARE(NAME,SIZE)       do { } while(0)
#define CA_DEFINE(TYPE,NAME,SIZE)   TYPE NAME[SIZE]
#define CA_REF(NAME,INDEX)          NAME[INDEX]

#else /* ifndef EF_NO_EFENCE */

#ifdef __cplusplus
extern "C" {
#endif

/* global Electric Fence variables */
extern int  EF_PROTECT_BELOW;
extern int  EF_ALIGNMENT;
extern int  EF_FILL;
extern struct _EF_Slot * _ef_allocList;


#ifndef _EFENCE_ENUMS
#define _EFENCE_ENUMS

enum _EF_Allocator
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
};

enum _EF_FailReturn
{
    EF_FAIL_NULL
  , EF_FAIL_ENV
};

#endif /* _EFENCE_ENUMS */


void   _eff_init(void);


#ifndef EF_NO_LEAKDETECTION

void * _eff_allocate(size_t alignment, size_t userSize, int protectBelow, int fillByte, int protectAllocList, enum _EF_Allocator allocator, enum _EF_FailReturn fail, const char * filename, int lineno);
void   _eff_deallocate(void * baseAdr, int protectAllocList, enum _EF_Allocator allocator, const char * filename, int lineno);

void * _eff_malloc(size_t size, const char * filename, int lineno);
void * _eff_calloc(size_t elemCount, size_t elemSize, const char * filename, int lineno);
void   _eff_free(void * baseAdr, const char * filename, int lineno);
void * _eff_memalign(size_t alignment, size_t userSize, const char * filename, int lineno);
void * _eff_realloc(void * baseAdr, size_t newSize, const char * filename, int lineno);
void * _eff_valloc(size_t size, const char * filename, int lineno);
char * _eff_strdup(const char *str, const char * filename, int lineno);
void * _eff_memcpy(void *dest, const void *src, size_t size, const char * filename, int lineno);
char * _eff_strcpy(char *dest, const char *src, const char * filename, int lineno);
char * _eff_strncpy(char *dest, const char *src, size_t size, const char * filename, int lineno);
char * _eff_strcat(char *dest, const char *src, const char * filename, int lineno);
char * _eff_strncat(char *dest, const char *src, size_t size, const char * filename, int lineno);


void  EF_newFrame(void);
void  EF_delFrame(void);

#define malloc(SIZE)                _eff_malloc(SIZE, __FILE__, __LINE__)
#define calloc(ELEMCOUNT, ELEMSIZE) _eff_calloc(ELEMCOUNT, ELEMSIZE, __FILE__, __LINE__)
#define free(BASEADR)               _eff_free(BASEADR, __FILE__, __LINE__)
#define memalign(ALIGNMENT, SIZE)   _eff_memalign(ALIGNMENT, SIZE, __FILE__, __LINE__)
#define realloc(BASEADR, NEWSIZE)   _eff_realloc(BASEADR, NEWSIZE, __FILE__, __LINE__)
#define valloc(SIZE)                _eff_valloc(SIZE, __FILE__, __LINE__)
#define strdup(STR)                 _eff_strdup(STR, __FILE__, __LINE__)
#define memcpy(DEST, SRC, SIZE)     _eff_memcpy(DEST, SRC, SIZE, __FILE__, __LINE__)
#define strcpy(DEST, SRC)           _eff_strcpy(DEST, SRC, __FILE__, __LINE__)
#define strncpy(DEST, SRC, SIZE)    _eff_strncpy(DEST, SRC, SIZE, __FILE__, __LINE__)
#define strcat(DEST, SRC)           _eff_strcat(DEST, SRC, __FILE__, __LINE__)
#define strncat(DEST, SRC, SIZE)    _eff_strncat(DEST, SRC, SIZE, __FILE__, __LINE__)

#else /* EF_NO_LEAKDETECTION */

void * _eff_allocate(size_t alignment, size_t userSize, int protectBelow, int fillByte, int protectAllocList, enum _EF_Allocator allocator, enum _EF_FailReturn fail);
void   _eff_deallocate(void * baseAdr, int protectAllocList, enum _EF_Allocator allocator);

void * _eff_malloc(size_t size);
void * _eff_calloc(size_t elemCount, size_t elemSize);
void   _eff_free(void * baseAdr);
void * _eff_memalign(size_t alignment, size_t userSize);
void * _eff_realloc(void * baseAdr, size_t newSize);
void * _eff_valloc(size_t size);
char * _eff_strdup(const char *str);
void * _eff_memcpy(void *dest, const void *src, size_t size);
char * _eff_strcpy(char *dest, const char *src);
char * _eff_strncpy(char *dest, const char *src, size_t size);
char * _eff_strcat(char *dest, const char *src);
char * _eff_strncat(char *dest, const char *src, size_t size);

#define EF_newFrame() do { } while(0)
#define EF_delFrame() do { } while(0)

#endif /* EF_NO_LEAKDETECTION */

/* some special assert */
void _eff_assert(const char * exprstr, const char * filename, int lineno);

#define EF_ASSERT(EXPR)    (  (EXPR) || ( _eff_assert(#EXPR, __FILE__, __LINE__), 0 )  )


/*
 * protection of functions return address
 */
#ifdef __GNUC__

#define EF_FN_PROT_START      const void * EF_RET_ADDR = __builtin_return_address(0); {

#define EF_FN_PROT_END        } EF_ASSERT( __builtin_return_address(0) == EF_RET_ADDR );

#define EF_FN_PROT_RET(EXPR)  do {  EF_ASSERT( __builtin_return_address(0) == EF_RET_ADDR );  return( EXPR ); }  while (0)
#define EF_FN_PROT_RET_VOID() do {  EF_ASSERT( __builtin_return_address(0) == EF_RET_ADDR );  return;         }  while (0)

#else

#define EF_FN_PROT_START      int aiEF_PROT[ 4 ] = { 'E', 'F', 'P', 'R' }; {

#define EF_FN_PROT_END        } EF_ASSERT( 'E'==aiEF_PROT[0] && 'F'==aiEF_PROT[1] && 'P'==aiEF_PROT[2] && 'R'==aiEF_PROT[3] );

#define EF_FN_PROT_RET(EXPR)  do {  EF_ASSERT( 'E'==aiEF_PROT[0] && 'F'==aiEF_PROT[1] && 'P'==aiEF_PROT[2] && 'R'==aiEF_PROT[3] );  return( EXPR ); }  while (0)
#define EF_FN_PROT_RET_VOID() do {  EF_ASSERT( 'E'==aiEF_PROT[0] && 'F'==aiEF_PROT[1] && 'P'==aiEF_PROT[2] && 'R'==aiEF_PROT[3] );  return;         }  while (0)

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
NAME[ EF_ASSERT( (NAME ## _checkedidx = (INDEX)) < NAME ## _checkedsize ), NAME ## _checkedidx ]


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* end ifdef EF_NO_EFENCE */
