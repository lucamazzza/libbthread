#include <stdio.h>
#include <stdlib.h>

#include "bthread.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

void *simple_thread_func(void *arg) {
    int *val = (int *)arg;
    *val = 42;
    return arg;
}

void test_thread_create() {
    TEST("Thread creation");
    bthread_t thread;
    bthread_attr_t attr = {};
    int arg = 0;
    int result = bthread_create(&thread, &attr, simple_thread_func, &arg);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(thread > 0, "Expected valid thread ID");
    PASS();
}

int main() {
    PRINT_TITLE("Running bthread Tests");
    test_thread_create();
    PRINT_TITLE("Test Results");
    printf("Tests run:    %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    return (test_count == test_passed) ? 0 : 1;
}
