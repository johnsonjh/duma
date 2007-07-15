#include <stdlib.h>

int main()
{
  int *pi;
  int i;
#ifdef DUMA_EXPLICIT_INIT
  duma_init();
#endif
  pi = (int*)malloc(10*sizeof(int));
  for(i=0; i<10; ++i)
    pi[i] = i;
  return 0;
}

