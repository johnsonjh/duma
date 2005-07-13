
/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
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
 * internal header file
 */


#ifndef EF_SEM_INC_H
#define EF_SEM_INC_H

#ifndef EF_NO_THREAD_SAFETY

#ifdef __cplusplus
extern "C" {
#endif

void EF_get_sem(void);
void EF_rel_sem(void);

#ifdef __cplusplus
} /* extern "C" */
#endif


#define EF_GET_SEMAPHORE()      EF_get_sem()
#define EF_RELEASE_SEMAPHORE()  EF_rel_sem()

#else  /* EF_NO_THREAD_SAFETY */

#define EF_GET_SEMAPHORE()      do { } while (0)
#define EF_RELEASE_SEMAPHORE()  do { } while (0)

#endif /* EF_NO_THREAD_SAFETY */

#endif /* EF_SEM_INC_H */
