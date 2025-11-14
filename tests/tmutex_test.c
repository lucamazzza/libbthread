#include <stdio.h>
#include <stdlib.h>

#include "bthread.h"
#include "bthread_private.h"
#include "tmutex.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

/* ==================== BASIC TESTS ==================== */

void test_mutex_init() {
    TEST("Mutex initialization");
    bthread_mutex_t mutex;
    int result = bthread_mutex_init(&mutex, NULL);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(mutex.owner == NULL, "Expected owner NULL");
    ASSERT_TEST(mutex.waiting_queue == NULL, "Expected empty queue");
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

/* ==================== LOCK/UNLOCK TESTS ==================== */

bthread_mutex_t test_mutex;

void *simple_lock_thread(void *arg) {
    int *flag = (int *)arg;
    bthread_mutex_lock(&test_mutex);
    *flag = 1;
    bthread_mutex_unlock(&test_mutex);
    return NULL;
}

void test_lock_unlock() {
    TEST("Mutex lock and unlock");
    bthread_mutex_init(&test_mutex, NULL);
    
    int ret = bthread_mutex_lock(&test_mutex);
    ASSERT_TEST(ret == 0, "Lock failed");
    ASSERT_TEST(test_mutex.owner != NULL, "Owner should be set");
    
    ret = bthread_mutex_unlock(&test_mutex);
    ASSERT_TEST(ret == 0, "Unlock failed");
    ASSERT_TEST(test_mutex.owner == NULL, "Owner should be NULL");
    
    bthread_mutex_destroy(&test_mutex);
    PASS();
}

void test_lock_unlock_with_thread() {
    TEST("Mutex lock/unlock in thread");
    bthread_mutex_init(&test_mutex, NULL);
    
    bthread_t t;
    int flag = 0;
    
    bthread_create(&t, NULL, simple_lock_thread, &flag);
    bthread_join(t, NULL);
    
    ASSERT_TEST(flag == 1, "Thread didn't execute critical section");
    
    bthread_mutex_destroy(&test_mutex);
    PASS();
}

/* ==================== MUTUAL EXCLUSION TEST ==================== */

volatile int shared_counter = 0;

void *increment_thread(void *arg) {
    int iterations = *(int *)arg;
    for (int i = 0; i < iterations; i++) {
        bthread_mutex_lock(&test_mutex);
        shared_counter++;
        bthread_yield();
        bthread_mutex_unlock(&test_mutex);
    }
    return NULL;
}

void test_mutual_exclusion() {
    TEST("Mutex mutual exclusion");
    #define NUM_THREADS 4
    #define ITERATIONS 50
    
    bthread_mutex_init(&test_mutex, NULL);
    shared_counter = 0;
    
    bthread_t threads[NUM_THREADS];
    int iterations = ITERATIONS;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        bthread_create(&threads[i], NULL, increment_thread, &iterations);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        bthread_join(threads[i], NULL);
    }
    
    ASSERT_TEST(shared_counter == NUM_THREADS * ITERATIONS, 
                "Mutual exclusion violated");
    
    bthread_mutex_destroy(&test_mutex);
    PASS();
}

/* ==================== TRYLOCK TESTS ==================== */

void *trylock_success_thread(void *arg) {
    bthread_mutex_t *mutex = (bthread_mutex_t *)arg;
    int ret = bthread_mutex_trylock(mutex);
    if (ret == 0) {
        bthread_mutex_unlock(mutex);
        return (void *)1L;
    }
    return (void *)0L;
}

void test_trylock_success() {
    TEST("Mutex trylock success");
    bthread_mutex_t mutex;
    bthread_mutex_init(&mutex, NULL);
    
    bthread_t t;
    bthread_create(&t, NULL, trylock_success_thread, &mutex);
    
    void *retval;
    bthread_join(t, &retval);
    
    ASSERT_TEST((long)retval == 1, "Trylock should succeed");
    
    bthread_mutex_destroy(&mutex);
    PASS();
}

void *trylock_fail_thread(void *arg) {
    bthread_mutex_t *mutex = (bthread_mutex_t *)arg;
    bthread_sleep(10); // Let main thread lock first
    int ret = bthread_mutex_trylock(mutex);
    return (void *)(long)ret;
}

void test_trylock_fail() {
    TEST("Mutex trylock failure");
    bthread_mutex_t mutex;
    bthread_mutex_init(&mutex, NULL);
    
    bthread_mutex_lock(&mutex);
    
    bthread_t t;
    bthread_create(&t, NULL, trylock_fail_thread, &mutex);
    
    void *retval;
    bthread_join(t, &retval);
    
    bthread_mutex_unlock(&mutex);
    
    ASSERT_TEST((long)retval == -1, "Trylock should fail when locked");
    
    bthread_mutex_destroy(&mutex);
    PASS();
}

/* ==================== BLOCKING TESTS ==================== */

volatile int execution_order[10];
volatile int order_index = 0;

void *blocking_thread(void *arg) {
    int id = *(int *)arg;
    bthread_mutex_lock(&test_mutex);
    execution_order[order_index++] = id;
    bthread_sleep(20);
    bthread_mutex_unlock(&test_mutex);
    return NULL;
}

void test_blocking_behavior() {
    TEST("Mutex blocking behavior");
    #define NUM_BLOCK_THREADS 3
    
    bthread_mutex_init(&test_mutex, NULL);
    order_index = 0;
    
    bthread_t threads[NUM_BLOCK_THREADS];
    int ids[NUM_BLOCK_THREADS] = {1, 2, 3};
    
    for (int i = 0; i < NUM_BLOCK_THREADS; i++) {
        bthread_create(&threads[i], NULL, blocking_thread, &ids[i]);
    }
    
    for (int i = 0; i < NUM_BLOCK_THREADS; i++) {
        bthread_join(threads[i], NULL);
    }
    
    // All threads should have executed
    ASSERT_TEST(order_index == NUM_BLOCK_THREADS, "Not all threads executed");
    
    bthread_mutex_destroy(&test_mutex);
    PASS();
}

/* ==================== NESTED LOCK TEST ==================== */

bthread_mutex_t mutex1, mutex2;

void *nested_lock_thread(void *arg) {
    int *value = (int *)arg;
    
    bthread_mutex_lock(&mutex1);
    (*value)++;
    bthread_mutex_lock(&mutex2);
    (*value)++;
    bthread_mutex_unlock(&mutex2);
    bthread_mutex_unlock(&mutex1);
    
    return NULL;
}

void test_nested_locks() {
    TEST("Nested mutex locks");
    bthread_mutex_init(&mutex1, NULL);
    bthread_mutex_init(&mutex2, NULL);
    
    bthread_t t;
    int value = 0;
    
    bthread_create(&t, NULL, nested_lock_thread, &value);
    bthread_join(t, NULL);
    
    ASSERT_TEST(value == 2, "Nested locks didn't work correctly");
    
    bthread_mutex_destroy(&mutex1);
    bthread_mutex_destroy(&mutex2);
    PASS();
}

/* ==================== STRESS TEST ==================== */

void *stress_mutex_thread(void *arg) {
    volatile int *counter = (int *)arg;
    for (int i = 0; i < 20; i++) {
        bthread_mutex_lock(&test_mutex);
        (*counter)++;
        if (i % 3 == 0) bthread_yield();
        bthread_mutex_unlock(&test_mutex);
        if (i % 5 == 0) bthread_sleep(5);
    }
    return NULL;
}

void test_mutex_stress() {
    TEST("Mutex stress test");
    #define STRESS_THREADS 6
    
    bthread_mutex_init(&test_mutex, NULL);
    
    bthread_t threads[STRESS_THREADS];
    volatile int counter = 0;
    
    for (int i = 0; i < STRESS_THREADS; i++) {
        bthread_create(&threads[i], NULL, stress_mutex_thread, (void *)&counter);
    }
    
    for (int i = 0; i < STRESS_THREADS; i++) {
        bthread_join(threads[i], NULL);
    }
    
    ASSERT_TEST(counter == STRESS_THREADS * 20, "Stress test failed");
    
    bthread_mutex_destroy(&test_mutex);
    PASS();
}

/* ==================== MAIN ==================== */

int main() {
    printf("\n");
    PRINT_TITLE("  TMutex Tests  ");
    printf("\n");
    
    PRINT_TITLE(" Basic Operations ");
    test_mutex_init();
    test_mutex_destroy();
    
    PRINT_TITLE(" Lock/Unlock Operations ");
    test_lock_unlock();
    test_lock_unlock_with_thread();
    
    PRINT_TITLE(" Mutual Exclusion ");
    test_mutual_exclusion();
    
    PRINT_TITLE(" Trylock Operations ");
    test_trylock_success();
    test_trylock_fail();
    
    PRINT_TITLE(" Blocking Behavior ");
    test_blocking_behavior();
    
    PRINT_TITLE(" Nested Locks ");
    test_nested_locks();
    
    PRINT_TITLE(" Stress Test ");
    test_mutex_stress();
    
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
