/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
 * Copyright (C) 2002 Hayati Ayguen <hayati.ayguen@epost.de>, Procitec GmbH
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
 * internal header file
 */

#ifndef _EFENCE_INTERNAL_
#define _EFENCE_INTERNAL_
#ifndef NDEBUG
/* -> DEBUG */


#include "efence.h"


#ifdef __cplusplus
/* C declarations from within C++ */
#undef  EF_EXTERN_C
#define EF_EXTERN_C
extern "C" {
#else
#undef EF_EXTERN_C
#define EF_EXTERN_C extern
#endif


/* assure that our functions do not use these macros */

#ifdef malloc
#undef malloc
#endif

#ifdef calloc
#undef calloc
#endif

#ifdef realloc
#undef realloc
#endif

#ifdef free
#undef free
#endif

#ifdef memalign
#undef memalign
#endif



/*
 * ef_number is the largest unsigned integer we'll need. On systems that
 * support 64-bit pointers, this may be "unsigned long long".
 */
#if defined(USE_LONG_LONG)
typedef unsigned long long	ef_number;
#else
typedef unsigned long		ef_number;
#endif

/*
 * NBBY is the number of bits per byte. Some systems define it in
 * <sys/param.h> .
 */
#ifndef	NBBY
#define	NBBY	8
#endif

EF_EXTERN_C  void   Page_AllowAccess(void * address, size_t size);
EF_EXTERN_C  void * Page_Create(size_t size);
EF_EXTERN_C  void   Page_Delete(void * address, size_t size);
EF_EXTERN_C  void   Page_DenyAccess(void * address, size_t size);
EF_EXTERN_C  size_t Page_Size(void);

EF_EXTERN_C  void   EF_Abort(const char * message, ...);
EF_EXTERN_C  void   EF_Exit(const char * message, ...);
EF_EXTERN_C  void   EF_Print(const char * message, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NDEBUG */
#endif /* _EFENCE_INTERNAL_ */
