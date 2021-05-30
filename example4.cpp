#include <stdlib.h>
#include <duma.h>

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
    delete pi;  // this line should produce error, cause pi was allocated with malloc()
    return 0;
}
