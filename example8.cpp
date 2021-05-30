#include <dumapp.h>

int main()
{
    int *pi;
    int i;
#ifdef DUMA_EXPLICIT_INIT
    duma_init();
#endif
    pi = new int[10];
    for(i=0; i<10; ++i)
        pi[i] = i;
    delete pi;  // this line should produce error, cause pi was allocated with new[]()
    // allocation source should be reported, cause dumapp.h is included
    return 0;
}
