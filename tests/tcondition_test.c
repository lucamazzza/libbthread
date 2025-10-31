#include <stdio.h>
#include <stdlib.h>

#include "tcondition.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

void test_condition_init() {
    TEST("Condition variable initialization");
    bthread_cond_t cond;
    int result = bthread_cond_init(&cond, NULL);
    ASSERT_TEST(result == 0, "Expected result 0");
    PASS();
}

void test_condition_destroy() {
    TEST("Condition variable destroy");
    bthread_cond_t cond;
    bthread_cond_init(&cond, NULL);
    int result = bthread_cond_destroy(&cond);
    ASSERT_TEST(result == 0, "Expected result 0");
    PASS();
}

int main() {
    PRINT_TITLE("Running tcondition Tests");
    test_condition_init();
    test_condition_destroy();
    PRINT_TITLE("Test Results");
    printf("Tests run:    %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    return (test_count == test_passed) ? 0 : 1;
}
