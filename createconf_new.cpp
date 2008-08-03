
/*
 * DUMA - Red-Zone memory allocator.
 *
 * Copyright (C) 2008 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 *
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

#include <stdlib.h>
#include <new>

#ifdef __cplusplus
  #define DUMA_EXTERN_C   extern "C"
#else
  #define DUMA_EXTERN_C   extern
#endif

DUMA_EXTERN_C
int get_new0strategy(void)
{
  /* test for behaviour of operator new with size 0 */
  int op_new_0_strategy;
  int * piNullPtrA;
  int * piNullPtrB;

  piNullPtrA = (int*) operator new(0);
  piNullPtrB = (int*) operator new(0);

  if ( !piNullPtrA )
    op_new_0_strategy = 0;
  else if ( piNullPtrA == piNullPtrB )
    op_new_0_strategy = 1;
  else
    op_new_0_strategy = 2;

  return op_new_0_strategy;
}


