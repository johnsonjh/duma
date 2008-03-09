/*
 * source URL:
 * http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=67838
 */

/*
 * Author:  Petter Reinholdtsen <pere@hungry.com>
 * Date:    2002-02-13
 * Lisence: GPL
 *
 * Small test program to demonstrate efence crashing on multithreaded
 * programs.

 * Link it like this, and it crashes with segfault.
 *  cc -o efence-thread-test efence-thread-test.c  -lpthread -lefence
 * Remove the -lefence, and it work as it should.
 * The output from gdb when it fails:
 *
 * Program received signal SIGSEGV, Segmentation fault.
 * [Switching to Thread 1026 (LWP 1189)]
 * 0x400382c7 in memalign () from /usr/lib/libefence.so.0
 * (gdb) bt
 * #0  0x400382c7 in memalign () from /usr/lib/libefence.so.0
 * #1  0x4003873c in malloc () from /usr/lib/libefence.so.0
 * #2  0x400a0811 in fopen () from /lib/libc.so.6
 * #3  0x080488e8 in thread_func ()
 * #4  0x40027f0a in pthread_start_thread () from /lib/libpthread.so.0
 * #5  0x40027f51 in pthread_start_thread_event () from /lib/libpthread.so.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "duma.h"

static pthread_mutex_t mutex;
static volatile int threads_left = 2;

static void*
thread_func(void *arg)
{
  int i = 2000;
  char *name = (char*)arg;

  while (--i)
    {
      if (pthread_mutex_lock(&mutex))
        {
          fprintf(stderr, "error: %s failed to lock mutex.\n", name);
          exit(1);
        }
      printf ("%s : %d\n", name, i);
      if (pthread_mutex_unlock(&mutex))
        {
          fprintf(stderr, "error: %s failed to lock mutex.\n", name);
          exit(1);
        }

      {
        /* Try to trigger efence error */
        FILE *fp = fopen("/etc/resolv.conf", "r");
        if (NULL != fp)
          {
            char buf[1024];
            fread(buf, sizeof(buf), 1, fp);
            fclose(fp);
          }
      }
    }

  --threads_left;

  return NULL;
}

static void*
idle_func(void* arg)
{
  while (threads_left) ;

  /* NOTREACHED */
  return NULL;
}

int
main(int argc,
     char **argv)
{

  pthread_t hello_thread, goodbye_thread;

  pthread_mutex_init(&mutex, NULL);

  if (pthread_create(&hello_thread, NULL, thread_func, (void*)"hello"))
    {
      fprintf(stderr, "Failed to create hello thread\n");
      exit(1);
    }

  if (pthread_create(&goodbye_thread, NULL, thread_func, (void*)"goodbye"))
    {
      fprintf(stderr, "Failed to create hello thread\n");
      exit(1);
    }

  idle_func(NULL);

  pthread_mutex_destroy(&mutex);

  /* NOTREACHED */
  return 0;
}

