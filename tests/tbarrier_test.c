#include <stdio.h>
#include <stdlib.h>

#include "tbarrier.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

void test_barrier_init() {
    TEST("Barrier initialization");
    bthread_barrier_t barrier;
    int result = bthread_barrier_init(&barrier, NULL, 3);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(barrier.count == 3, "Expected count 3");
    ASSERT_TEST(barrier.size == 0, "Expected size 0");
    PASS();
}

void test_barrier_destroy() {
    TEST("Barrier destroy");
    bthread_barrier_t barrier;
    bthread_barrier_init(&barrier, NULL, 3);
    int result = bthread_barrier_destroy(&barrier);
    ASSERT_TEST(result == 0, "Expected result 0");
    PASS();
}

int main() {
    PRINT_TITLE("Running tbarrier Tests");
    test_barrier_init();
    test_barrier_destroy();
    PRINT_TITLE("Test Results");
    printf("Tests run:    %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    return (test_count == test_passed) ? 0 : 1;
}
