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
 * header file for inclusion from YOUR application code
 */

#ifndef _EFENCE_H_
#define _EFENCE_H_

#ifdef NDEBUG
/* -> RELEASE */

#include <stdlib.h>

/* enable these macros even in release code, but do nothing */
#define EF_newFrame() do { } while(0)
#define EF_delFrame() do { } while(0)

#else /* NDEBUG */
/* -> DEBUG */

#include <sys/types.h>
#include <stddef.h>

/* remove previous definitions */
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

/* C declarations from within C++ */
#ifdef	__cplusplus
#define	C_LINKAGE	"C"
#else
#define	C_LINKAGE
#endif

/* default declarations */
extern C_LINKAGE void *malloc(size_t size);
extern C_LINKAGE void *calloc(size_t elemCount, size_t elemSize);
extern C_LINKAGE void *realloc(void * baseAdr, size_t newSize);
extern C_LINKAGE void  free(void * baseAdr);

#ifndef EF_NO_LEAKDETECTION

extern C_LINKAGE void *_eff_malloc(size_t size, const char * filename, int lineno);
extern C_LINKAGE void *_eff_calloc(size_t elemCount, size_t elemSize, const char * filename, int lineno);
extern C_LINKAGE void *_eff_realloc(void * baseAdr, size_t newSize, const char * filename, int lineno);
extern C_LINKAGE void  _eff_free(void * baseAdr);

extern C_LINKAGE void  EF_newFrame(void);
extern C_LINKAGE void  EF_delFrame(void);

#define malloc(SIZE)                _eff_malloc(SIZE, __FILE__, __LINE__)
#define calloc(ELEMCOUNT, ELEMSIZE) _eff_calloc(ELEMCOUNT, ELEMSIZE, __FILE__, __LINE__)
#define realloc(BASEADR, NEWSIZE)   _eff_realloc(BASEADR, NEWSIZE, __FILE__, __LINE__)
#define free(BASEADR)               _eff_free(BASEADR)

#else /* EF_NO_LEAKDETECTION */

#define EF_newFrame() do { } while(0)
#define EF_delFrame() do { } while(0)

#endif /* EF_NO_LEAKDETECTION */

#endif /* NDEBUG */
#endif /* _EFENCE_H_ */
