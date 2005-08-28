#include <malloc.h>
#include <new>
#include <efence.h>
#include <efencepp.h>

void main()
{
  int *pi = new int[10];
  int i;
  for(i=0; i<10; ++i)
    pi[i] = i;
  delete []pi;
}
