

#include <new>
#include "efencepp.h"



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


int main( int argc, char ** argv )
{

#if 1

  int i;

  CA_DEFINE(int,acTest,20);



  for (i=0; i<100; ++i)

    CA_REF(acTest,i) = i;



#elif 0

  int * x, *y;

  test *z;



  x = (int*)malloc( sizeof(int) );

  y = new int;

  z = new test[2];



  free( x );

  delete y;

  delete []z;



#elif 1

  test * x = new test(10);

  test *ax = new test[2];

  testtest *y = new testtest;

  delete y;

  delete []ax;


  delete x;


#endif


  return 0;

}
