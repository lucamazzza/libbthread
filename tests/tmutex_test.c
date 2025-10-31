#include <stdio.h>
#include <stdlib.h>

#include "tmutex.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

void test_mutex_init() {
    TEST("Mutex initialization");
    bthread_mutex_t mutex;
    int result = bthread_mutex_init(&mutex, NULL);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(mutex.owner == NULL, "Expected owner NULL");
    PASS();
}

void test_mutex_destroy() {
    TEST("Mutex destroy");
    bthread_mutex_t mutex;
    bthread_mutex_init(&mutex, NULL);
    int result = bthread_mutex_destroy(&mutex);
    ASSERT_TEST(result == 0, "Expected result 0");
    PASS();
}

int main() {
    PRINT_TITLE("Running tmutex Tests");
    test_mutex_init();
    test_mutex_destroy();
    PRINT_TITLE("Test Results");
    printf("Tests run:    %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    return (test_count == test_passed) ? 0 : 1;
}
