/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 2002 Hayati Ayguen <hayati.ayguen@epost.de>, Procitec GmbH
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
 */

#ifndef _EFENCEPP_H_
#define _EFENCEPP_H_

#include "efence.h"

#ifndef EF_NO_CPP
/* avoid usage of C++ operator replacements in C code */

#ifndef	__cplusplus
#error compile with a C++ compiler, or define EF_NO_CPP to remove this error
#endif


/* remove any previous definitions */
#ifdef NEW_ELEM
#undef NEW_ELEM
#endif

#ifdef NEW_ARRAY
#undef NEW_ARRAY
#endif

#ifdef DEL_ELEM
#undef DEL_ELEM
#endif

#ifdef DEL_ARRAY
#undef DEL_ARRAY
#endif


#ifdef NDEBUG
/* -> RELEASE */

#define NEW_ELEM(TYPE)          new TYPE
#define NEW_ARRAY(TYPE,COUNT)   new (TYPE)[COUNT]

#define DEL_ELEM(PTR)           delete PTR
#define DEL_ARRAY(PTR)          delete []PTR

#else /* NDEBUG */
/* -> DEBUG */

#ifndef EF_NO_LEAKDETECTION

#define NEW_ELEM(TYPE)    ( \
                            _ef_ovr_file = __FILE__, \
                            _ef_ovr_line = __LINE__, \
                            _ef_ovr_fl = 1, \
                            new TYPE \
                          )

#define NEW_ARRAY(TYPE, COUNT)    ( \
                            _ef_ovr_file = __FILE__, \
                            _ef_ovr_line = __LINE__, \
                            _ef_ovr_fl = 1, \
                            new TYPE[COUNT] \
                          )

#else

#define NEW_ELEM(TYPE)    new TYPE
#define NEW_ARRAY(TYPE, COUNT)  new TYPE[COUNT]

#endif

#define DEL_ELEM(PTR)           delete PTR
#define DEL_ARRAY(PTR)          delete []PTR


#endif /* NDEBUG */

#endif /*	EF_NO_CPP */

#endif /* _EFENCEPP_H_ */
