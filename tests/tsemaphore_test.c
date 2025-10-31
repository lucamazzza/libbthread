#include <stdio.h>
#include <stdlib.h>

#include "tsemaphore.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

void test_semaphore_init() {
    TEST("Semaphore initialization");
    bthread_sem_t sem;
    int result = bthread_sem_init(&sem, 0, 1);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(sem.value == 1, "Expected initial value 1");
    PASS();
}

void test_semaphore_init_zero() {
    TEST("Semaphore initialization with zero");
    bthread_sem_t sem;
    int result = bthread_sem_init(&sem, 0, 0);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(sem.value == 0, "Expected initial value 0");
    PASS();
}

void test_semaphore_destroy() {
    TEST("Semaphore destroy");
    bthread_sem_t sem;
    bthread_sem_init(&sem, 0, 1);
    int result = bthread_sem_destroy(&sem);
    ASSERT_TEST(result == 0, "Expected result 0");
    PASS();
}

void test_semaphore_post() {
    TEST("Semaphore post");
    bthread_sem_t sem;
    bthread_sem_init(&sem, 0, 0);
    int result = bthread_sem_post(&sem);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(sem.value == 1, "Expected value 1 after post");
    bthread_sem_destroy(&sem);
    PASS();
}

int main() {
    PRINT_TITLE("Running tsemaphore Tests");
    test_semaphore_init();
    test_semaphore_init_zero();
    test_semaphore_destroy();
    test_semaphore_post();
    PRINT_TITLE("Test Results");
    printf("Tests run:    %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    return (test_count == test_passed) ? 0 : 1;
}
