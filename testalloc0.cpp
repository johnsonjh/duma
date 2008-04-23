
#include <stdlib.h>
#include <stdio.h>
#include <new>

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

  delete piNullPtrA;
  delete piNullPtrB;

  return op_new_0_strategy;
}

int get_newVec0strategy(void)
{
  /* test for behaviour of operator new with size 0 */
  int op_new_0_strategy;
  int * piNullPtrA;
  int * piNullPtrB;

  piNullPtrA = new int[0];
  piNullPtrB = new int[0];

  if ( !piNullPtrA )
    op_new_0_strategy = 0;
  else if ( piNullPtrA == piNullPtrB )
    op_new_0_strategy = 1;
  else
    op_new_0_strategy = 2;

  delete []piNullPtrA;
  delete []piNullPtrB;

  return op_new_0_strategy;
}

int get_malloc0strategy(void)
{
  int op_new_0_strategy;
  int * piNullPtrA;
  int * piNullPtrB;

  piNullPtrA = (int*)malloc(0);
  piNullPtrB = (int*)malloc(0);

  if ( !piNullPtrA )
    op_new_0_strategy = 0;
  else if ( piNullPtrA == piNullPtrB )
    op_new_0_strategy = 1;
  else
    op_new_0_strategy = 2;

  free(piNullPtrA);
  free(piNullPtrB);

  return op_new_0_strategy;
}


int main(int argc, char *argv[])
{
  int test = 0;
  int strategy;

  if ( argc >= 1 )
    test = atoi( argv[1] );

  switch (test)
  {
    default:
    case 0:  printf("testing malloc(0) .. "); strategy = get_malloc0strategy();  break;
    case 1:  printf("testing new(0) .. ");    strategy = get_new0strategy();     break;
    case 2:  printf("testing new[0] .. ");    strategy = get_newVec0strategy();  break;
  }

  printf("result is %d\n", strategy);

  return 0;
}

