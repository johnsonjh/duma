
/*
 * test program from
 * Debian Bug report logs - #241156
 * electric-fence (2.1.13-0.1) breaks with 2.6.x kernel
 */

/* $Id$ */
/* gcc foo.c -pthread -lefence -g -ggdb -o foo */

#if (defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__))
/* (defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__)) */
/* no pthread library on windows!
 * but on cygwin
 * so skip this test for now
 */

int main(int argc, char *argv[])
{
  printf("Test not implemented for Win32, Cygwin and Mingw\n");
  return 0;
}

#else

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "duma.h"

volatile int iKillThreads = 0;

void* poster(void* arg)
{
  char* foo = NULL;
  for( ; !iKillThreads; )
  {
    foo = (char*) malloc(4096);
    if (foo)
      free(foo);
  }
  return(NULL);
}

int main(int argc, char *argv[])
{
  int iSleepTime = 60;  /* in seconds; default = 10 sec */
  pthread_t ida;
  pthread_t idb;

  if ( argc >= 2 )
  {
    iSleepTime = atoi(argv[1]);
    if ( iSleepTime <= 0 )
      iSleepTime = 10;
  }

  fprintf(stdout, "running 2 threads for %d secs ..", iSleepTime);
  fflush(stdout);

  pthread_create(&ida, NULL, poster, NULL);
  pthread_create(&idb, NULL, poster, NULL);

  sleep(iSleepTime);
  fprintf(stdout, "..done\n");
  iKillThreads = 1;
  fprintf(stdout, "wating threads to end ..");
  fflush(stdout);
  pthread_join(ida, NULL);
  pthread_join(idb, NULL);
  fprintf(stdout, "..done\n");
  return 0;
}

#endif
