#include <malloc.h>

void main()
{
  int *pi = (int*)malloc(10*sizeof(int));
  int i;
  for(i=0; i<11; ++i)
    pi[i] = i;
}

