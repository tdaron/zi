#include "editor.h"
#include "tests_buffers.c"

#define TUNIT_IMPLEMENTATION
#include <tunit.h>

Editor editor;

int main()
{
    testsuite_t* buffers = t_registerTestSuite("buffers");
    register_buffers_test(buffers);

    return t_runSuites();
    return 0;
}
