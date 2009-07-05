
#include <stdlib.h>
#include <stdio.h>

/* #include "duma.h" */

int main( int argc, char** argv )
{
  int* ptr1 = NULL;
  int* ptr2 = NULL;
  int* ptr3 = NULL;
  int* ptr4 = NULL;
  int* ptr5 = NULL;
  int* ptr6 = NULL;
  int i;

  ptr1 = (int*) malloc( 100000 * sizeof(int) );
  for( i = 0; i < 100000; ++i )
    ptr1[i] = i;

  ptr2 = (int*) malloc( 100000 * sizeof(int) );
  for( i = 0; i < 100000; ++i )
    ptr2[i] = i;

  ptr3 = (int*) malloc( 1000000 * sizeof(int) );
  for( i = 0; i < 1000000; ++i )
    ptr3[i] = i;

  ptr4 = (int*) malloc( 1000000 * sizeof(int) );
  for( i = 0; i < 1000000; ++i )
    ptr4[i] = i;

  ptr5 = (int*) malloc( 10000000 * sizeof(int) );
  for( i = 0; i < 10000000; ++i )
    ptr5[i] = i;

  ptr6 = (int*) malloc( 10000000 * sizeof(int) );
  for( i = 0; i < 10000000; ++i )
    ptr6[i] = i;

  free(ptr1);
  free(ptr2);
  free(ptr3);
  free(ptr4);
  free(ptr5);
  free(ptr6);

  return 0;
}


