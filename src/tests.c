#define TUNIT_IMPLEMENTATION
#include "tunit.h"

void nice_test() {
  t_assert_int(1, ==, 1);
}


int main(int argc, char** argv) {
  testsuite_t* suite = t_registerTestSuite("basics");
  t_addTestToSuite(suite,"Nice Test", nice_test);
  t_runSuites(argc, argv);
  return 0;
}
