#include <noduma.h>
#include <iostream>
#include <string>
#include <dumapp.h>

using namespace std;

class Test {
public:
  int a;
  string stdstr;
  
  Test() {
    a=2;
    stdstr = "test";
  }

};

int main() {
  cout << "Hello world!" << endl;

  {
  Test* pI = new Test[10];
  cerr << "Let's correctly delete an array of 10 objects " << endl;
  delete [] pI;
  cerr << "Done" << endl;
  }

  cerr << "There should be 0 errors in this run" << endl;
  return 0;
}
