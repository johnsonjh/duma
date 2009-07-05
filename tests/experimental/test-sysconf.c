
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void sc_print()
{
  long sc_memprot   = sysconf(_SC_MEMORY_PROTECTION);
  long sc_shell     = sysconf(_SC_SHELL);
  long sc_page_size = sysconf(_SC_PAGE_SIZE);
  long sc_pagesize  = sysconf(_SC_PAGESIZE);
  long sc_physpages = sysconf(_SC_PHYS_PAGES);
  long sc_avpages   = sysconf(_SC_AVPHYS_PAGES);

  printf("sysconf(_SC_MEMORY_PROTECTION) = %ld\n", sc_memprot);
  printf("sysconf(_SC_SHELL)             = %ld\n", sc_shell);
  printf("sysconf(_SC_PAGE_SIZE)         = %ld\n", sc_page_size);
  printf("sysconf(_SC_PAGESIZE)          = %ld\n", sc_pagesize);
  printf("sysconf(_SC_PHYS_PAGES)        = %ld\n", sc_physpages);
  printf("sysconf(_SC_AVPHYS_PAGES)      = %ld\n", sc_avpages);
}


int main( int argc, char *argv[] )
{
  int * p;
  int i;
  sc_print();
  printf("\nallocated %d kB\n", 4096*sizeof(int) );
  p = (int*)malloc( 1024 * 4096 * sizeof(int) );
  sc_print();

  printf("\ninitialized memory\n");
  for (i=0; i<1024*4096; ++i)
    p[i] = 0;
  sc_print();
  
  if ( p )
    free( p );
  return 0;
}

