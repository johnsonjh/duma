
#include <stdio.h>

#include <new>
#include <fstream>

#ifndef DUMA_SO_LIBRARY
#include "dumapp.h"
#endif


#if 0

class test
{
public:
  test()
  {
    x = 0;
  }

  test(int m)
  {
    x = m;
  }

  ~test()
  {
    x = 0;
  }

  int x;
};

class testtest
{
public:
  testtest()
  {
    y = new test;     //NEW_ELEM( test );   //new test();
  }

  testtest(int m)
  {
    y = new test(m);  //NEW_ELEM( test(m) );  //new test(m);
  }

  ~testtest()
  {
    delete y;
  }

  test *y;
};
#endif

int main( int argc, char ** argv )
{
#if 0
  int * x = 0;
  int ret;
  ret = posix_memalign(&x, 2048, sizeof(int) );
  if ( !ret )
    *x = 1;
  free(x);
#elif 0
#include "noduma.h"
  int * x = 0;
  x = new int[10];
  x[0] = 0;
  delete x;
#elif 0
	std::fstream *f = new std::fstream();
	delete f;

#elif 0
  /* this test should fail */
  int i;
  CA_DEFINE(int,acTest,20);

  for (i=0; i<100; ++i)
    CA_REF(acTest,i) = i;
#elif 0
  /* this test should not fail */
  int *y, *z;

  y = new int;
  z = new int[2];
  DEL_ARRAY( z );
  DEL_ARRAY( z );
  DEL_ELEM( y );
  DEL_ELEM( y );

#elif 1

  int *y, *z;

  y = new int;
  z = new int[2];
  delete []z;
  delete y;

#elif 0
  int *y, *z;

  y = new int;
  z = new int[2];
  DEL_ARRAY(z);
  DEL_ELEM(y);

  //y = new int;
  //DEL_ELEM(y);
  //DEL_ELEM(y);

  //y = new(__FILE__,__LINE__) int;
  //delete (__FILE__, __LINE__, y);

  //y = new(std::nothrow) int;
  //delete (std::nothrow, y);

  //y = new(std::nothrow, __FILE__, __LINE__) int;
  //delete (std::nothrow, __FILE__, __LINE__, y);
#elif 0
  /* this test should not fail */
  int *y;
  y = NEW_ARRAY( int, 2 );
  DEL_ARRAY( y );
#elif 0
  /* this test should not fail */
  int * x, *y;
  test *z;
  x = (int*)malloc( sizeof(int) );
  y = new int;
  z = new test[2];
  free( x );
  delete y;
  delete []z;
#elif 0
  /* this test should not fail */
  test * x = new test(10);
  test *ax = new test[2];
  testtest *y = new testtest;
  delete y;
  delete []ax;
  delete x;
#endif
  return 0;

}

