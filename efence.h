
/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
 * Copyright (C) 2002-2004 Hayati Ayguen <hayati.ayguen@epost.de>, Procitec GmbH
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
#undef malloc
#undef calloc
#undef realloc
#undef free
#undef EF_newFrame
#undef EF_delFrame

#undef EF_STACK_PROT_START
#undef EF_STACK_PROT_END

#undef CA_DECLARE
#undef CA_DEFINE
#undef CA_REF

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

#ifndef _EFENCE_SLOT_STATE
#define _EFENCE_SLOT_STATE
/*
 * Enum _EF_Slot_State indicates the status of a slot
 * Warning: Also have a look at ALLOCATED_MASK in efence.c
 */
enum _EF_Slot_State
{
    EFST_NOT_IN_USE      = 0   /* Available to represent a malloc buffer. */
  , EFST_FREE            = 1   /* A free buffer. */
  , EFST_ALLOC_MALLOC    = 2   /* allocated through C "malloc()", "calloc()", "realloc()" .. */
  , EFST_ALLOC_NEW_ELEM  = 4   /* allocated through C++ "new"   */
  , EFST_ALLOC_NEW_ARRAY = 8   /* allocated through C++ "new[]" */
  , EFST_INTERNAL_USE    = 16  /* A buffer used internally by malloc(). */
  , EFST_PROTECTED       = 32  /* A freed buffer that can not be allocated again. */
};
#endif /* _EFENCE_SLOT_STATE */


#ifndef EF_NO_LEAKDETECTION

void * _eff_malloc(size_t size, enum _EF_Slot_State, const char * filename, int lineno);
void * _eff_calloc(size_t elemCount, size_t elemSize, enum _EF_Slot_State, const char * filename, int lineno);
void * _eff_realloc(void * baseAdr, size_t newSize, enum _EF_Slot_State, const char * filename, int lineno);
void   _eff_free(void * baseAdr, enum _EF_Slot_State);

void  EF_newFrame(void);
void  EF_delFrame(void);

#define malloc(SIZE)                _eff_malloc(SIZE, EFST_ALLOC_MALLOC, __FILE__, __LINE__)
#define calloc(ELEMCOUNT, ELEMSIZE) _eff_calloc(ELEMCOUNT, ELEMSIZE, EFST_ALLOC_MALLOC, __FILE__, __LINE__)
#define realloc(BASEADR, NEWSIZE)   _eff_realloc(BASEADR, NEWSIZE, EFST_ALLOC_MALLOC, __FILE__, __LINE__)
#define free(BASEADR)               _eff_free(BASEADR, EFST_ALLOC_MALLOC)

#else /* EF_NO_LEAKDETECTION */

#define EF_newFrame() do { } while(0)
#define EF_delFrame() do { } while(0)

#endif /* EF_NO_LEAKDETECTION */

/* some special assert */
void _eff_assert(const char * exprstr, const char * filename, int lineno);

#define EF_ASSERT(EXPR)    (  (EXPR) || ( _eff_assert(#EXPR, __FILE__, __LINE__), 0 )  )


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
