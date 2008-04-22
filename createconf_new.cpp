
#include <stdlib.h>
#include <new>

#ifdef __cplusplus
  #define DUMA_EXTERN_C   extern "C"
#else
  #define DUMA_EXTERN_C   extern
#endif

DUMA_EXTERN_C
int get_new0strategy(void)
{
  /* test for behaviour of operator new with size 0 */
  int op_new_0_strategy;
  int * piNullPtrA;
  int * piNullPtrB;

  piNullPtrA = (int*) operator new(0);
  piNullPtrB = (int*) operator new(0);

  if ( !piNullPtrA )
    op_new_0_strategy = 0;
  else if ( piNullPtrA == piNullPtrB )
    op_new_0_strategy = 1;
  else
    op_new_0_strategy = 2;

  return op_new_0_strategy;
}


