
/*
 * DUMA - Red-Zone memory allocator.
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
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
 * DUMA confidence tests.
 * Make sure all of the various functions of DUMA work correctly.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef WIN32
  #include <unistd.h>
#else
  #include <io.h>
#endif
#include <setjmp.h>
#include <signal.h>

#include "duma.h"


#ifndef  PAGE_PROTECTION_VIOLATED_SIGNAL
/* changed default in code below to use two signals: SIGSEGV and SIGBUS */
/* #define  PAGE_PROTECTION_VIOLATED_SIGNAL  SIGSEGV */
#endif

struct diagnostic
{
  int           (*test)(void);      /* pointer to some test function returning int */
  int           expectedStatus;     /* expected return value/status */
  const char  * explanation;        /* explanation of that test */
};

extern int  DUMA_PROTECT_BELOW;
extern int  DUMA_ALIGNMENT;

static jmp_buf  env;

/*
 * There is still too little standardization of the arguments and return
 * type of signal handler functions.
 */
static void
segmentationFaultHandler(
  int signalNumber
#if ( defined(_AIX) )
  , ...
#endif
)
{
#ifdef WIN32
  longjmp(env, 1);
#else
  siglongjmp(env, 1);
#endif
}


static int
gotSegmentationFault(int (*test)(void))
{
#ifndef WIN32
  sigset_t newmask, oldmask;
  int savemask;
#endif
#ifdef PAGE_PROTECTION_VIOLATED_SIGNAL
  void (*oldhandler)(int)         = SIG_ERR;
#else
  void (*oldSIGSEGVhandler)(int)  = SIG_ERR;
#ifndef WIN32
  void (*oldSIGBUShandler)(int)   = SIG_ERR;
#endif
#endif
  int status;

#ifdef WIN32
  if ( 0 == setjmp(env) )
#else
  if ( 0 == sigsetjmp(env, savemask) )
#endif
  {
#ifndef WIN32
    /* unblock signal and save previous signal mask */
    sigemptyset(&newmask);
  #ifdef PAGE_PROTECTION_VIOLATED_SIGNAL
    sigaddset(&newmask, PAGE_PROTECTION_VIOLATED_SIGNAL);
  #else
    sigaddset(&newmask, SIGSEGV);
    sigaddset(&newmask, SIGBUS);
  #endif
    sigprocmask(SIG_UNBLOCK, &newmask, &oldmask);
#endif

#ifdef PAGE_PROTECTION_VIOLATED_SIGNAL
    oldhandler = signal(PAGE_PROTECTION_VIOLATED_SIGNAL, segmentationFaultHandler);
#else
    oldSIGSEGVhandler = signal(SIGSEGV, segmentationFaultHandler);
  #ifndef WIN32
    oldSIGBUShandler  = signal(SIGBUS, segmentationFaultHandler);
  #endif
#endif

    status = (*test)();
  }
  else
    status = 1;

  /* install previous signal handler */

#ifdef PAGE_PROTECTION_VIOLATED_SIGNAL
  if (SIG_ERR != oldhandler)
    signal(PAGE_PROTECTION_VIOLATED_SIGNAL, oldhandler);
#else
  if (SIG_ERR != oldSIGSEGVhandler)
    signal(SIGSEGV, oldSIGSEGVhandler);
#ifndef WIN32
  if (SIG_ERR != oldSIGBUShandler)
    signal(SIGBUS, oldSIGBUShandler);
#endif
#endif

#ifndef WIN32
  /* restore signal mask */
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
#endif
  return status;
}


static char *  allocation;
/* c is global so that assignments to it won't be optimized out. */
char  c;

static int
testSizes(void)
{
  /*
   * If DUMA_ADDR can't hold all of the bits of a void *,
   * have the user call createconf.
   */
  return ( sizeof(DUMA_ADDR) < sizeof(void *) );
}

static int
allocateMemory(void)
{
  allocation = (char *)malloc(1);

  if ( allocation != 0 )
    return 0;
  else
    return 1;
}

static int
freeMemory(void)
{
  free(allocation);
  allocation = 0;
  return 0;
}

static int
protectBelow(void)
{
  DUMA_PROTECT_BELOW = 1;
  return 0;
}

static int
protectAbove(void)
{
  DUMA_PROTECT_BELOW = 0;
  return 0;
}

static int
read0(void)
{
  c = *allocation;
  return 0;
}

static int
write0(void)
{
  *allocation = 1;
  return 0;
}

static int
read1(void)
{
  c = allocation[1];
  return 0;
}

static int
readMinus1(void)
{
  c = allocation[-1];
  return 0;
}


static struct diagnostic diagnostics[] =
{
  {
    testSizes, 0,      "Please add -DLONG_LONG to the compiler flags and recompile."
  },
#if 1
  {
    protectAbove, 0,   "Protect above: This sets DUMA to protect\n"
                       "the upper boundary of a malloc buffer, rather than the lower boundary."
  },
  {
    allocateMemory, 0, "Allocation 1: This test allocates a single byte of memory."
  },
  {
    read0, 0,          "Read valid memory 1: This test reads the allocated memory."
  },
  {
    write0, 0,         "Write valid memory 1: This test writes the allocated memory."
  },
  {
    readMinus1, 0,     "Read underrun: This test reads before the beginning of the buffer."
  },
  {
    read1, 1,          "Read overrun: This test reads beyond the end of the buffer."
  },
  {
    freeMemory, 0,     "Free memory 1: This test frees the allocated memory."
  },
#endif
#if 1
  {
    protectBelow, 0,   "Protect below: This sets DUMA to protect\n"
                       "the lower boundary of a malloc buffer, rather than the upper boundary."
  },
  {
    allocateMemory, 0, "Allocation 2: This allocates memory with the lower boundary protected."
  },
  {
    read0, 0,          "Read valid memory 2: This test reads the allocated memory."
  },
  {
    write0, 0,         "Write valid memory 2: This test writes the allocated memory."
  },
  {
    readMinus1, 1,     "Read underrun: This test reads before the beginning of the buffer."
  },
  {
    freeMemory, 0,     "Free memory 2: This test frees the allocated memory."
  },
#endif
  {
    0, 0, 0
  }
};


static const char  failedTest[] = "DUMA confidence test failed.\n";
static const char  newline = '\n';


int
main(int argc, char * * argv)
{
  static const struct diagnostic *  diag = diagnostics;
  int testno;

#ifdef DUMA_EXPLICIT_INIT
  duma_init();
#endif

  DUMA_PROTECT_BELOW = 0;
  DUMA_ALIGNMENT = DUMA_MIN_ALIGNMENT;

  allocation = 0;

  for (testno=0; diag->explanation != 0; ++testno, ++diag)
  {
    int status;

#if 0
    write(0, diag->explanation, strlen(diag->explanation));
    write(0, &newline, 1);
#endif

    status = gotSegmentationFault(diag->test);

    if ( status != diag->expectedStatus )
    {
      /*
       * Don't use stdio to print here, because stdio
       * uses malloc() and we've just proven that malloc()
       * is broken. Also, use _exit() instead of exit(),
       * because _exit() doesn't flush stdio.
       */
      write(2, failedTest, sizeof(failedTest) - 1);
      write(2, diag->explanation, strlen(diag->explanation));
      write(2, &newline, 1);
      _exit(-1);
    }
  }

  /* avoid memory leak */
  if (allocation)
    freeMemory();

  return 0;
}
