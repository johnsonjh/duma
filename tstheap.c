#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <memory.h>
#include "efence.h"

/*
 * This is a simple program to exercise the allocator. It allocates and frees
 * memory in a pseudo-random fashion. It should run silently, using up time
 * and resources on your system until you stop it or until it has gone
 * through TEST_DURATION (or the argument) iterations of the loop.
 */

#ifdef	__cplusplus
extern "C"
#else
extern
#endif
double drand48(void); /* For pre-ANSI C systems */

#ifdef WIN32
#define FAKE_DRAND48
#endif

#define	POOL_SIZE	1024
#define	LARGEST_BUFFER	30000
#define	TEST_DURATION	1000000

void *	pool[POOL_SIZE];

#ifdef	FAKE_DRAND48
/*
 * Add -DFAKE_DRAND48 to your compile flags if your system doesn't
 * provide drand48().
 */

#ifndef	ULONG_MAX
#define	ULONG_MAX	~(1L)
#endif

double
drand48(void)
{
#ifdef WIN32
  return (double)rand()/((double)RAND_MAX);
#else
	return (random() / (double)ULONG_MAX);
#endif
}
#endif

int
main(int argc, char * * argv)
{
  int  count;
  int  duration = TEST_DURATION;

  if ( argc >= 2 )
    duration = atoi(argv[1]);

  for ( count = 0; count < POOL_SIZE; count++ )
    pool[count] = 0;

  for ( count = 0; count < duration; count++ )
  {
    int       pool_idx;
    void  * * element;
    size_t    size;

    pool_idx =(int)(drand48() * POOL_SIZE);
    if (pool_idx >=0 && pool_idx<POOL_SIZE)
    {
      element  = &pool[pool_idx];
      size     = (size_t)(drand48() * (LARGEST_BUFFER + 1));

      if ( *element )
      {
        free( *element );
        *element = 0;
      }
      
      if ( size > 0 )
      {
        *element = malloc(size);
        /* really use it, so that the system has to use real memory */
        memset( *element, -1, size );
      }
    }
  }

#if 1
  /* don't forget to free the allocated memory, else the
     confidence test won't pass - without having set
     "EF_NO_LEAKDETECTION" preprocessor definition
  */
  for ( count = 0; count < POOL_SIZE; count++ )
  {
    if ( pool[count] )
      free( pool[count] );
  }
#endif

  return 0;
}
