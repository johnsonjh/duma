
#include "efencepp.h"

/*
#define EF_MAX_DEPTH 10
const char * EF_OVR_FILE[EF_MAX_DEPTH];
int EF_OVR_LINE[EF_MAX_DEPTH];
int EF_OVR_NUM = -1;

#define EF_NEW(TYPE)  ( EF_OVR_FILE[++EF_OVR_NUM] = __FILE__, EF_OVR_LINE[EF_OVR_NUM] = __LINE__, \
                      new TYPE )

*/

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
    y = NEW_ELEM( test );   //new test();
  }

  testtest(int m)
  {
    y = NEW_ELEM( test(m) );  //new test(m);
  }

  ~testtest()
  {
    //delete y;
  }

  test *y;
};


int main( int argc, char ** argv )
{
  test * x = NEW_ELEM( test(10) );

  //test *ax = NEW_ELEM( test[2] );
  test *ax = NEW_ARRAY( test, 2 );

  testtest *y = NEW_ELEM( testtest );

  DEL_ELEM(y);

  DEL_ARRAY( ax);
  DEL_ELEM(x);
  return 0;
}
