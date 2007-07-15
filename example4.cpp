#include <stdlib.h>
#include <new>
#include <duma.h>
#include <dumapp.h>

int main()
{
  int *pi = (int*)malloc(10*sizeof(int));
  int i;
  for(i=0; i<10; ++i)
    pi[i] = i;
  delete pi;
  return 0;
}
