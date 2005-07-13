
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
 * internal implementation file
 * contains thread safety functions (semaphore lock/release)
 */


#ifndef EF_SEM_INC_H
#define EF_SEM_INC_H

#ifndef EF_NO_THREAD_SAFETY

#ifndef WIN32
  #include <pthread.h>
  #include <semaphore.h>
#else
  #define WIN32_LEAN_AND_MEAN 1
  #include <windows.h>
  #include <winbase.h>
#endif


/*
 * EF_sem is a semaphore used to allow one thread at a time into
 * these routines.
 * Also, we use semInited as a boolean to see if we should be
 * using the semaphore.
 * semThread is set to the thread id of the thread that currently
 * has the semaphore so that when/if it tries to get the semaphore
 * again (realloc calling malloc/free) - nothing will happen to the
 * semaphore.
 * semDepth is used to keep track of how many times the same thread
 * gets the semaphore - so we know when it is actually freed.
 */



#ifndef WIN32
static sem_t      EF_sem = { 0 };
static pthread_t  semThread = (pthread_t) 0;

#else /* WIN32 */
static SECURITY_ATTRIBUTES  semSecAttr;

#ifndef UNICODE
#define SEM_NAME_TYPE char
#define SEM_STRCPY    strcpy
#define SEM_STRCAT    strcat
static char       semObjectName[] = "EFence_";
#else
#define SEM_NAME_TYPE wchar_t
#define SEM_STRCPY    wcscpy
#define SEM_STRCAT    wcscat
static wchar_t    semObjectName[] = L"EFence_";
#endif

static DWORD      semThread = 0;
static HANDLE     semHandle = 0;
#endif

static int        semInited = 0;
static int        semDepth  = 0;


/* define prototypes to avoid warnings of BorlandC++ compiler */

static void EF_init_sem(void);
static void EF_get_sem(void);
static void EF_rel_sem(void);


static
void EF_init_sem()
{
#ifndef WIN32
  if (sem_init(&EF_sem, 0, 1) >= 0)
    semInited = 1;
#else
  SEM_NAME_TYPE   semLocalName[32];
  SEM_NAME_TYPE   acPID[16];
  DWORD pid;

  pid = GetCurrentProcessId();
  SEM_STRCPY(semLocalName, semObjectName);
  /* append ProcessId() to get inter-process unique semaphore name */
  acPID[0] = 'A' + (SEM_NAME_TYPE)( (pid >> 28) & 0x0F );
  acPID[1] = 'A' + (SEM_NAME_TYPE)( (pid >> 24) & 0x0F );
  acPID[2] = 'A' + (SEM_NAME_TYPE)( (pid >> 20) & 0x0F );
  acPID[3] = 'A' + (SEM_NAME_TYPE)( (pid >> 16) & 0x0F );
  acPID[4] = 'A' + (SEM_NAME_TYPE)( (pid >> 12) & 0x0F );
  acPID[5] = 'A' + (SEM_NAME_TYPE)( (pid >>  8) & 0x0F );
  acPID[6] = 'A' + (SEM_NAME_TYPE)( (pid >>  4) & 0x0F );
  acPID[7] = 'A' + (SEM_NAME_TYPE)( (pid      ) & 0x0F );
  acPID[8] = 0;
  SEM_STRCAT( semLocalName, acPID );

  semSecAttr.nLength = sizeof(semSecAttr);
  semSecAttr.lpSecurityDescriptor = NULL;
  semSecAttr.bInheritHandle = FALSE;

  semHandle = CreateSemaphore( &semSecAttr      /* pointer to security attributes */
                               , 1              /* initial count */
                               , 10             /* maximum count */
                               , semLocalName   /* pointer to semaphore-object name */
                             );
  semInited = 1;
#endif
}


static
void EF_get_sem()
{
  if (!semInited)     EF_init_sem();    /* initialize if necessary */
  if (!semInited)     EF_Abort("\nCouldn't initialise semaphore");

#ifndef WIN32
  if ( pthread_self() != semThread )
  {
    while (sem_wait(&EF_sem) < 0) ;   /* wait for the semaphore. */
    semThread = pthread_self();       /* let everyone know who has the semaphore. */
  }
#else
  if ( GetCurrentThreadId() != semThread )
  {
    while (WaitForSingleObject(semHandle, 1000) != WAIT_OBJECT_0) ; /* wait for the semaphore. */
    semThread = GetCurrentThreadId(); /* let everyone know who has the semaphore. */
  }
#endif
  ++semDepth;                         /* increment semDepth - push one stack level */
}


static
void EF_rel_sem()
{
  if (!semInited)     EF_Abort("\nSemaphore isn't initialised");
  if (!semThread)     EF_Abort("\nSemaphore isn't owned by this thread");
  if (semDepth <= 0)  EF_Abort("\nSemaphore isn't locked");

  if (!(--semDepth))              /* decrement semDepth - popping one stack level */
  {
#ifndef WIN32
    semThread = (pthread_t) 0;        /* zero this before actually free'ing the semaphore. */
    if (sem_post(&EF_sem) < 0)
#else
    semThread = 0;                    /* zero this before actually free'ing the semaphore. */
    if (0 == ReleaseSemaphore(semHandle, 1 /* amount to add to current count */, NULL) )
#endif
      EF_Abort("Failed to post the semaphore.");
  }
}

#define EF_GET_SEMAPHORE()      EF_get_sem()
#define EF_RELEASE_SEMAPHORE()  EF_rel_sem()

#else  /* EF_NO_THREAD_SAFETY */

#define EF_GET_SEMAPHORE()      do { } while (0)
#define EF_RELEASE_SEMAPHORE()  do { } while (0)

#endif /* EF_NO_THREAD_SAFETY */

#endif /* EF_SEM_INC_H */
