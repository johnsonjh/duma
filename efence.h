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

/*
#ifndef _EFENCE_H_
#define _EFENCE_H_
*/

/* for enabling inclusion of efence.h after inclusion of efencint.h */
#undef EF_newFrame
#undef EF_delFrame
#undef EF_MALLOC_RETTYPE


#ifdef NDEBUG
/* -> RELEASE */

#include <stdlib.h>

/* enable these macros even in release code, but do nothing */
#define EF_newFrame() do { } while(0)
#define EF_delFrame() do { } while(0)

#else /* NDEBUG */
/* -> DEBUG */


#include <stdlib.h>

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


#ifdef __cplusplus
extern "C" {
#endif

#ifndef EF_NO_CPP

extern const char * _ef_ovr_file;
extern int _ef_ovr_line;
extern int _ef_ovr_fl;

#endif


#if 0
/* default declarations */
void * malloc(size_t size);
void * calloc(size_t elemCount, size_t elemSize);
void * realloc(void * baseAdr, size_t newSize);
void   free(void * baseAdr);
#endif

#ifndef EF_NO_LEAKDETECTION

void * _eff_malloc(size_t size, const char * filename, int lineno);
void * _eff_calloc(size_t elemCount, size_t elemSize, const char * filename, int lineno);
void * _eff_realloc(void * baseAdr, size_t newSize, const char * filename, int lineno);
void   _eff_free(void * baseAdr);

void  EF_newFrame(void);
void  EF_delFrame(void);

#define malloc(SIZE)                _eff_malloc(SIZE, __FILE__, __LINE__)
#define calloc(ELEMCOUNT, ELEMSIZE) _eff_calloc(ELEMCOUNT, ELEMSIZE, __FILE__, __LINE__)
#define realloc(BASEADR, NEWSIZE)   _eff_realloc(BASEADR, NEWSIZE, __FILE__, __LINE__)
#define free(BASEADR)               _eff_free(BASEADR)

#else /* EF_NO_LEAKDETECTION */

#define EF_newFrame() do { } while(0)
#define EF_delFrame() do { } while(0)

#endif /* EF_NO_LEAKDETECTION */

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NDEBUG */
/* #endif */ /* _EFENCE_H_ */

