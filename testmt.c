
/*
 * test program from
 * Debian Bug report logs - #241156
 * electric-fence (2.1.13-0.1) breaks with 2.6.x kernel
 */

/* $Id$ */
/* gcc foo.c -pthread -lefence -g -ggdb -o foo */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* poster(void* arg)
{
  char* foo = NULL;
  for(;;)
  {
    foo = (char*) malloc(4096);
    if (foo)
      free(foo);
  }
}

int main(int argc, char *argv[])
{
  int i;
  pthread_t id;

  for(i =0; i <2; i++)
    pthread_create(&id, NULL, poster, NULL);

  sleep(10000);
}
