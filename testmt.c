
/*
 * test program from
 * Debian Bug report logs - #241156
 * electric-fence (2.1.13-0.1) breaks with 2.6.x kernel
 *
 * changed to compile on Windows by Hayati Ayguen
 */

/* $Id$ */
/* gcc foo.c -pthread -lefence -g -ggdb -o foo */


/* (defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__)) */
#if ( defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) )

#include <windows.h>

#else

#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define HAVE_PTHREADS

#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "duma.h"


volatile int iKillThreads = 0;

#define MAX_NUM_THREADS   32

volatile int aiCounter[MAX_NUM_THREADS];

/* thread function
 * permanently allocate, initialize and deallocate in a loop
 */
#ifdef  HAVE_PTHREADS
void* poster(void* arg)
#else
DWORD WINAPI poster( LPVOID arg )
#endif
{
  char* foo = NULL;
  int iThreadNo = *((int*)arg);

#if 0
  /* sleep a bit, that other threads can get started
   * before producing high load
   */
#ifdef  HAVE_PTHREADS
  sleep(1);  /* wait 1 sec  */
#else
  Sleep(1000);
#endif
#endif

  for( ; !iKillThreads; )
  {
    foo = (char*)malloc(4096);
    if (foo)
    {
      memset(foo, 0, 4096);
      free(foo);
    }
    ++ aiCounter[iThreadNo];
  }

#ifdef  HAVE_PTHREADS
  pthread_exit(NULL);
#endif
  return 0;
}


#define MAX_NUM_THREADS   32

int main(int argc, char *argv[])
{
  int iSleepTime  = 60; /* in seconds; default = 30 sec */
  int iNumThreads = 4;
  int i;

#ifdef  HAVE_PTHREADS
  pthread_t tId[MAX_NUM_THREADS];
#else
  HANDLE    tId[MAX_NUM_THREADS];
#endif
  int tArg[MAX_NUM_THREADS];


#ifdef DUMA_EXPLICIT_INIT
  /* necessary on some platforms!
   * like on Win32-Cygwin
   */
  duma_init();
#endif

  if ( argc > 1 )
  {
    i = atoi(argv[1]);
    if ( i > 0 )
      iSleepTime = i;
  }
  if ( argc > 2 )
  {
    i = atoi(argv[2]);
    if ( i > 0 )
      iNumThreads = i;
    if ( iNumThreads > MAX_NUM_THREADS )
      iNumThreads = MAX_NUM_THREADS;
  }

  fprintf(stdout, "running %d threads for %d secs ..", iNumThreads, iSleepTime);
  fflush(stdout);

  for ( i = 0; i < iNumThreads; ++i )
  {
    tArg[i] = i;
    aiCounter[i] = 0;
  }

#ifdef  HAVE_PTHREADS
  fprintf(stdout, "creating threads with pthread library .. \n");
  fflush(stdout);
  
  for ( i = 0; i < iNumThreads; ++i )
  {
    pthread_t *pt = &tId[i];
    int r = pthread_create( pt, NULL, poster, &tArg[i] );
    if ( r )
    {
      fprintf(stderr, "\nerror in pthread_create() for thread %d: ", i);
      switch(r)
      {
        case EAGAIN:  fprintf(stderr, "EAGAIN");   break;
        case EINVAL:  fprintf(stderr, "EINVAL");   break;
        case EPERM:   fprintf(stderr, "EPERM");    break;
        default:      fprintf(stderr, "unexpected!"); break;
      }
    }
  }
  fprintf(stdout, ".. creating done\n");
  fflush(stdout);

  /* sleep(iSleepTime); */
  for ( i = 0; i < iSleepTime; ++i )
  {
    sleep(1);  /* wait 1 sec  */
    fprintf(stdout, ".");
    fflush(stdout);
  }
#else
  fprintf(stdout, "creating threads with Win32 API calls\n");
  fflush(stdout);

  for ( i = 0; i < iNumThreads; ++i )
  {
    tId[i] = CreateThread( NULL    /* default security attributes */
                    , 0       /* use default stack size */
                    , poster  /* thread function name */
                    , &tArg[i]/* argument to thread function */
                    , 0       /* use default creation flags */
                    , NULL    /* returns the thread identifier */
                    );
    if ( NULL == tId[i] )
      fprintf(stderr, "\nerror in CreateThread() for thread %d: ", i);
  }
  /* Sleep(iSleepTime*1000); */
  for ( i =0; i < iSleepTime; ++i )
  {
    Sleep(1000);  /* wait 1000 ms  */
    fprintf(stdout, ".");
    fflush(stdout);
  }
#endif

  fprintf(stdout, "..done\n");
  iKillThreads = 1;
  fprintf(stdout, "wating threads to end ..");
  fflush(stdout);

#ifdef  HAVE_PTHREADS
  for ( i = 0; i < iNumThreads; ++i )
  {
    pthread_t *pt = &tId[i];
    int r = pthread_join( *pt, NULL );
    if ( r )
    {
      fprintf(stderr, "\nerror in pthread_join() for thread %d: ", i);
      switch(r)
      {
        case EINVAL:  fprintf(stderr, "EINVAL");   break;
        case ESRCH:   fprintf(stderr, "ESRCH");    break;
        case EDEADLK: fprintf(stderr, "EDEADLK");  break;
        default:      fprintf(stderr, "unexpected!"); break;
      }
    }
  }
#else
  // Wait until all threads have terminated.
  WaitForMultipleObjects( iNumThreads, tId, TRUE, INFINITE);
  for ( i = 0; i < iNumThreads; ++i )
    CloseHandle(tId[i]);
#endif
  fprintf(stdout, "..done\n");

  fprintf(stdout, "state:\n");
  for ( i = 0; i < iNumThreads; ++i )
    fprintf(stdout, "Thread %d did %d (de)allocations\n", i, aiCounter[i]);
  fflush(stdout);

  return 0;
}

