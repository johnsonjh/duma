#include <malloc.h>
#include <duma.h>

void main()
{
  int *pi = (int*)malloc(10*sizeof(int));
  int i;
  for(i=0; i<10; ++i)
    pi[i] = i;
}
