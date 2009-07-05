
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <errno.h>

#ifndef WIN32
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/mman.h>
#else
#error
#endif

#include "duma_config.h"
#include "paging.h"

int main(int argc, char *argv[])
{
  long int pagesize;
  long int pre_phys_pages, pre_avail_pages;
  long int post_phys_pages, post_avail_pages;
  long int alloccount = 1024;
  char * block;
  long int i;

  pagesize = sysconf(_SC_PAGESIZE);
  printf("page size: %ld\n", pagesize);

  if ( argc >= 1 )
  {
    int tmp = atoi( argv[1] );
    if ( tmp >= 0 && tmp <= ((1 << 31)/pagesize) )
      alloccount = tmp;
  }

  pre_phys_pages  = sysconf(_SC_PHYS_PAGES);
  pre_avail_pages = sysconf(_SC_AVPHYS_PAGES);

  printf("before allocation: %ld available of %ld pages\n", pre_avail_pages, pre_phys_pages);
  printf("                   %ld available of %ld kB\n"
         , ((pre_avail_pages*pagesize)/1024)
         , ((pre_phys_pages *pagesize)/1024)
         );

#if 0
  block = valloc( alloccount * pagesize );
#else
  block = Page_Create( alloccount * pagesize, 0 /*exitonfail*/, 1 /*printerror*/ );
#endif
  for ( i = 0; i < alloccount * pagesize; ++i )
    block[i] = (char)(i & 1);

  post_phys_pages  = sysconf(_SC_PHYS_PAGES);
  post_avail_pages = sysconf(_SC_AVPHYS_PAGES);

  printf("\nallocated and used %d pages (=%ld kB)\n\n", alloccount
        , ((alloccount*pagesize)/1024)
        );

  printf("after  allocation: %ld available of %ld pages\n", post_avail_pages, post_phys_pages);
  printf("                   %ld available of %ld kB\n"
         , ((post_avail_pages*pagesize)/1024)
         , ((post_phys_pages *pagesize)/1024)
         );

  printf("difference: %ld less available pages\n"
         , (pre_avail_pages - post_avail_pages)
         );
  printf("            %ld less available kB\n"
         , (((pre_avail_pages - post_avail_pages)*pagesize)/1024)
         );

  return 0;
}

