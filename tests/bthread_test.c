#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bthread.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

/* ==================== BASIC TESTS ==================== */

void *simple_thread_func(void *arg) {
    int *val = (int *)arg;
    *val = 42;
    return arg;
}

void test_thread_create() {
    TEST("Thread creation with attributes");
    bthread_t thread;
    bthread_attr_t attr = {};
    int arg = 0;
    int result = bthread_create(&thread, &attr, simple_thread_func, &arg);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(thread > 0, "Expected valid thread ID");
    PASS();
}

void test_thread_create_null_attr() {
    TEST("Thread creation with NULL attributes");
    bthread_t thread;
    int arg = 0;
    int result = bthread_create(&thread, NULL, simple_thread_func, &arg);
    ASSERT_TEST(result == 0, "Expected result 0 with NULL attr");
    ASSERT_TEST(thread > 0, "Expected valid thread ID");
    PASS();
}

void test_thread_join() {
    TEST("Thread join");
    bthread_t thread;
    bthread_attr_t attr = {};
    int arg = 0;
    bthread_create(&thread, &attr, simple_thread_func, &arg);
    void *retval;
    int result = bthread_join(thread, &retval);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(retval == &arg, "Expected return value to match argument");
    ASSERT_TEST(arg == 42, "Expected arg to be modified to 42");
    PASS();
}

void test_thread_join_null_retval() {
    TEST("Thread join with NULL return value");
    bthread_t thread;
    int arg = 0;
    bthread_create(&thread, NULL, simple_thread_func, &arg);
    int result = bthread_join(thread, NULL);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(arg == 42, "Expected arg to be modified");
    PASS();
}

/* ==================== MULTIPLE THREADS ==================== */

void *counter_thread(void *arg) {
    volatile int *counter = (int *)arg;
    for (int i = 0; i < 100; i++) {
        (*counter)++;
        bthread_yield();
    }
    return NULL;
}

void test_multiple_threads() {
    TEST("Multiple threads execution");
    #define NUM_THREADS 5
    bthread_t threads[NUM_THREADS];
    volatile int counters[NUM_THREADS] = {0};
    
    for (int i = 0; i < NUM_THREADS; i++) {
        bthread_create(&threads[i], NULL, counter_thread, (void *)&counters[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        bthread_join(threads[i], NULL);
        ASSERT_TEST(counters[i] == 100, "Counter value incorrect");
    }
    PASS();
}

/* ==================== YIELD TESTS ==================== */

void *yielding_thread(void *arg) {
    volatile int *flag = (int *)arg;
    for (int i = 0; i < 20; i++) {
        bthread_yield();
    }
    *flag = 1;
    return NULL;
}

void test_yield() {
    TEST("Thread yielding");
    bthread_t t1, t2;
    volatile int flag1 = 0, flag2 = 0;
    
    bthread_create(&t1, NULL, yielding_thread, (void *)&flag1);
    bthread_create(&t2, NULL, yielding_thread, (void *)&flag2);
    
    bthread_join(t1, NULL);
    bthread_join(t2, NULL);
    
    ASSERT_TEST(flag1 == 1 && flag2 == 1, "Threads didn't complete");
    PASS();
}

/* ==================== EXIT TESTS ==================== */

void *exiting_thread(void *arg) {
    *(int *)arg = 55;
    bthread_exit((void *)(intptr_t)77);
    // Should never reach here
    *(int *)arg = 99;
    return NULL;
}

void test_exit() {
    TEST("Thread exit");
    bthread_t t;
    int value = 0;
    void *retval;
    
    bthread_create(&t, NULL, exiting_thread, &value);
    bthread_join(t, &retval);
    
    ASSERT_TEST(value == 55, "Thread executed after exit");
    ASSERT_TEST((intptr_t)retval == 77, "Exit value incorrect");
    PASS();
}

void *immediate_exit_thread(void *arg) {
    (void)arg;
    bthread_exit((void *)(intptr_t)999);
    return NULL;
}

void test_immediate_exit() {
    TEST("Thread immediate exit");
    bthread_t t;
    void *retval;
    
    bthread_create(&t, NULL, immediate_exit_thread, NULL);
    bthread_join(t, &retval);
    
    ASSERT_TEST((intptr_t)retval == 999, "Immediate exit value incorrect");
    PASS();
}

/* ==================== SLEEP TESTS ==================== */

void *sleeping_thread(void *arg) {
    volatile int *marker = (int *)arg;
    *marker = 1;
    bthread_sleep(100);
    *marker = 2;
    return NULL;
}

void test_sleep() {
    TEST("Thread sleep");
    bthread_t t;
    volatile int marker = 0;
    
    bthread_create(&t, NULL, sleeping_thread, (void *)&marker);
    bthread_join(t, NULL);
    
    ASSERT_TEST(marker == 2, "Thread didn't complete sleep");
    PASS();
}

void *multi_sleep_thread(void *arg) {
    int *count = (int *)arg;
    for (int i = 0; i < 3; i++) {
        (*count)++;
        bthread_sleep(50);
    }
    return NULL;
}

void test_multiple_sleeps() {
    TEST("Multiple thread sleeps");
    bthread_t t1, t2;
    volatile int count1 = 0, count2 = 0;
    
    bthread_create(&t1, NULL, multi_sleep_thread, (void *)&count1);
    bthread_create(&t2, NULL, multi_sleep_thread, (void *)&count2);
    
    bthread_join(t1, NULL);
    bthread_join(t2, NULL);
    
    ASSERT_TEST(count1 == 3 && count2 == 3, "Sleep counts incorrect");
    PASS();
}

/* ==================== CANCELLATION TESTS ==================== */

void *cancellable_thread(void *arg) {
    volatile int *counter = (int *)arg;
    for (int i = 0; i < 1000; i++) {
        bthread_testcancel();
        (*counter)++;
        bthread_yield();
    }
    return NULL;
}

void test_cancel() {
    TEST("Thread cancellation");
    bthread_t t;
    volatile int counter = 0;
    
    bthread_create(&t, NULL, cancellable_thread, (void *)&counter);
    
    // Let it run a bit
    bthread_sleep(10);
    
    int ret = bthread_cancel(t);
    ASSERT_TEST(ret == 0, "Cancel failed");
    
    void *retval;
    bthread_join(t, &retval);
    
    ASSERT_TEST((intptr_t)retval == -1, "Cancel return value incorrect");
    ASSERT_TEST(counter < 1000, "Thread wasn't cancelled");
    PASS();
}

void *testcancel_thread(void *arg) {
    volatile int *counter = (int *)arg;
    for (int i = 0; i < 50; i++) {
        (*counter)++;
        bthread_testcancel();
    }
    return (void *)(intptr_t)200;
}

void test_testcancel_without_cancel() {
    TEST("Testcancel without cancel request");
    bthread_t t;
    volatile int counter = 0;
    void *retval;
    
    bthread_create(&t, NULL, testcancel_thread, (void *)&counter);
    bthread_join(t, &retval);
    
    ASSERT_TEST(counter == 50, "Counter incorrect");
    ASSERT_TEST((intptr_t)retval == 200, "Should complete normally");
    PASS();
}

/* ==================== EDGE CASES ==================== */

void *no_yield_thread(void *arg) {
    *(int *)arg = 777;
    return (void *)(intptr_t)888;
}

void test_no_yield() {
    TEST("Thread without yield");
    bthread_t t;
    int value = 0;
    void *retval;
    
    bthread_create(&t, NULL, no_yield_thread, &value);
    bthread_join(t, &retval);
    
    ASSERT_TEST(value == 777, "Value not set");
    ASSERT_TEST((intptr_t)retval == 888, "Return value incorrect");
    PASS();
}

void *return_null_thread(void *arg) {
    *(int *)arg = 123;
    return NULL;
}

void test_return_null() {
    TEST("Thread returning NULL");
    bthread_t t;
    int value = 0;
    void *retval = (void *)0xDEADBEEF;
    
    bthread_create(&t, NULL, return_null_thread, &value);
    bthread_join(t, &retval);
    
    ASSERT_TEST(value == 123, "Value not set");
    ASSERT_TEST(retval == NULL, "Return value should be NULL");
    PASS();
}

/* ==================== STRESS TEST ==================== */

void *stress_thread(void *arg) {
    volatile int *counter = (int *)arg;
    for (int i = 0; i < 100; i++) {
        (*counter)++;
        if (i % 5 == 0) bthread_yield();
        if (i % 10 == 0) bthread_sleep(5);
        bthread_testcancel();
    }
    return NULL;
}

void test_stress() {
    TEST("Stress test with many operations");
    #define STRESS_THREADS 8
    bthread_t threads[STRESS_THREADS];
    volatile int counters[STRESS_THREADS] = {0};
    
    for (int i = 0; i < STRESS_THREADS; i++) {
        bthread_create(&threads[i], NULL, stress_thread, (void *)&counters[i]);
    }
    
    for (int i = 0; i < STRESS_THREADS; i++) {
        bthread_join(threads[i], NULL);
        ASSERT_TEST(counters[i] == 100, "Stress test counter incorrect");
    }
    PASS();
}

/* ==================== MAIN ==================== */

int main() {
    printf("\n");
    PRINT_TITLE("  BThread Core Functionality Tests  ");
    printf("\n");
    
    PRINT_TITLE(" Basic Operations ");
    test_thread_create();
    test_thread_create_null_attr();
    test_thread_join();
    test_thread_join_null_retval();
    
    PRINT_TITLE(" Multiple Threads ");
    test_multiple_threads();
    
    PRINT_TITLE(" Yield Operations ");
    test_yield();
    
    PRINT_TITLE(" Exit Operations ");
    test_exit();
    test_immediate_exit();
    
    PRINT_TITLE(" Sleep Operations ");
    test_sleep();
    test_multiple_sleeps();
    
    PRINT_TITLE(" Cancellation ");
    test_cancel();
    test_testcancel_without_cancel();
    
    PRINT_TITLE(" Edge Cases ");
    test_no_yield();
    test_return_null();
    
    PRINT_TITLE(" Stress Tests ");
    test_stress();
    
    printf("\n");
    PRINT_TITLE(" Test Summary ");
    printf("Tests run:    %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    
    if (test_count == test_passed) {
        printf("\n\e[1;32m✓ All tests passed!\e[0m\n\n");
        return 0;
    } else {
        printf("\n\e[1;31m✗ Some tests failed\e[0m\n\n");
        return 1;
    }
}
