#include <malloc.h>
#include <new>
#include <duma.h>
#include <dumapp.h>
#define malloc(SIZE)                _duma_malloc(SIZE, __FILE__, __LINE__)

void main()
{
  int *pi = (int*)malloc(10*sizeof(int));
  int i;
  for(i=0; i<10; ++i)
    pi[i] = i;
  delete pi;
}
