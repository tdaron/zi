#include <buffer.h>
#include <tunit.h>

void test_basic_buffers()
{
    Buffer* b = new_buffer(NULL, NULL, 0);
    insert_string(b, "Hello, World !");
    t_assert_str_eq(get_raw_content(b), "Hello, World !");
    free_buffer(b);
}

void register_buffers_test(testsuite_t* buffers)
{
    t_addTestToSuite(buffers, "basic_buffers_test", test_basic_buffers);
}
