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
 * internal implementation file
 * replaces global C++ operators using C malloc/free functions (replaced by Electric Fence)
 */

#ifndef NDEBUG
#ifndef EF_NO_CPP

#ifndef	__cplusplus
#error compile with a C++ compiler, or define EF_NO_CPP to remove this error
#endif


#include <stdlib.h>
#include "efence.h"


/* memory stack for recursive new from constructors ! */
const char * _ef_ovr_file;
int _ef_ovr_line;
int _ef_ovr_fl = 0;


static const char unknown_file[] =
 "UNKNOWN (use #include \"efencepp.h\")";


/*
 * allocate memory for an array
 */
void * operator new[] (size_t size)
{
#ifndef EF_NO_LEAKDETECTION
  if (_ef_ovr_fl)
    return _eff_malloc(size, _ef_ovr_file, _ef_ovr_line);
  else
    return _eff_malloc(size, unknown_file, 0);
#else
  return malloc(size);
#endif
}


/*
 * free memory of an array
 */
void   operator delete[] (void* address)
{
  free(address);
}


/*
 * allocate memory for a single element
 */
void * operator new(size_t size)
{
#ifndef EF_NO_LEAKDETECTION
  if (_ef_ovr_fl)
    return _eff_malloc(size, _ef_ovr_file, _ef_ovr_line);
  else
    return _eff_malloc(size, unknown_file, 0);
#else
  return malloc(size);
#endif
}


/*
 * free memory of a single element
 */
void   operator delete(void* address)
{
  free(address);
}


#endif /* EF_NO_CPP */
#endif /* NDEBUG */
