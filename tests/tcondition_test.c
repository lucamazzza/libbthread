#include <stdio.h>
#include <stdlib.h>

#include "bthread.h"
#include "bthread_private.h"
#include "tcondition.h"
#include "tmutex.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

/* ==================== BASIC TESTS ==================== */

void test_condition_init() {
    TEST("Condition variable initialization");
    bthread_cond_t cond;
    int result = bthread_cond_init(&cond, NULL);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(cond.waiting_queue == NULL, "Expected empty queue");
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

/* ==================== SIGNAL TESTS ==================== */

bthread_cond_t test_cond;
bthread_mutex_t cond_mutex;
volatile int cond_flag = 0;

void *wait_signal_thread(void *arg) {
    int *result = (int *)arg;
    
    bthread_mutex_lock(&cond_mutex);
    while (cond_flag == 0) {
        bthread_cond_wait(&test_cond, &cond_mutex);
    }
    *result = cond_flag;
    bthread_mutex_unlock(&cond_mutex);
    
    return NULL;
}

void *signal_thread(void *arg) {
    (void)arg;
    bthread_sleep(50);
    
    bthread_mutex_lock(&cond_mutex);
    cond_flag = 1;
    bthread_cond_signal(&test_cond);
    bthread_mutex_unlock(&cond_mutex);
    
    return NULL;
}

void test_cond_signal() {
    TEST("Condition variable signal");
    
    bthread_cond_init(&test_cond, NULL);
    bthread_mutex_init(&cond_mutex, NULL);
    cond_flag = 0;
    
    bthread_t waiter, signaler;
    int result = 0;
    
    bthread_create(&waiter, NULL, wait_signal_thread, &result);
    bthread_create(&signaler, NULL, signal_thread, NULL);
    
    bthread_join(waiter, NULL);
    bthread_join(signaler, NULL);
    
    ASSERT_TEST(result == 1, "Signal didn't wake up waiter");
    
    bthread_cond_destroy(&test_cond);
    bthread_mutex_destroy(&cond_mutex);
    PASS();
}

/* ==================== BROADCAST TESTS ==================== */

void *broadcast_wait_thread(void *arg) {
    int *count = (int *)arg;
    
    bthread_mutex_lock(&cond_mutex);
    while (cond_flag == 0) {
        bthread_cond_wait(&test_cond, &cond_mutex);
    }
    (*count)++;
    bthread_mutex_unlock(&cond_mutex);
    
    return NULL;
}

void *broadcast_signal_thread(void *arg) {
    (void)arg;
    bthread_sleep(50);
    
    bthread_mutex_lock(&cond_mutex);
    cond_flag = 1;
    bthread_cond_broadcast(&test_cond);
    bthread_mutex_unlock(&cond_mutex);
    
    return NULL;
}

void test_cond_broadcast() {
    TEST("Condition variable broadcast");
    #define NUM_WAITERS 5
    
    bthread_cond_init(&test_cond, NULL);
    bthread_mutex_init(&cond_mutex, NULL);
    cond_flag = 0;
    
    bthread_t waiters[NUM_WAITERS];
    bthread_t broadcaster;
    volatile int counts[NUM_WAITERS] = {0};
    
    for (int i = 0; i < NUM_WAITERS; i++) {
        bthread_create(&waiters[i], NULL, broadcast_wait_thread, 
                      (void *)&counts[i]);
    }
    
    bthread_create(&broadcaster, NULL, broadcast_signal_thread, NULL);
    
    for (int i = 0; i < NUM_WAITERS; i++) {
        bthread_join(waiters[i], NULL);
    }
    bthread_join(broadcaster, NULL);
    
    // All waiters should have been woken up
    for (int i = 0; i < NUM_WAITERS; i++) {
        ASSERT_TEST(counts[i] == 1, "Not all threads woken by broadcast");
    }
    
    bthread_cond_destroy(&test_cond);
    bthread_mutex_destroy(&cond_mutex);
    PASS();
}

/* ==================== PRODUCER-CONSUMER WITH COND VAR ==================== */

volatile int buffer[10];
volatile int buffer_idx = 0;
volatile int produced = 0;
volatile int consumed = 0;

void *producer_cond_thread(void *arg) {
    int items = *(int *)arg;
    
    for (int i = 0; i < items; i++) {
        bthread_mutex_lock(&cond_mutex);
        while (buffer_idx >= 5) {  // Buffer full
            bthread_cond_wait(&test_cond, &cond_mutex);
        }
        buffer[buffer_idx++] = i;
        produced++;
        bthread_cond_signal(&test_cond);
        bthread_mutex_unlock(&cond_mutex);
        bthread_yield();
    }
    return NULL;
}

void *consumer_cond_thread(void *arg) {
    int items = *(int *)arg;
    
    for (int i = 0; i < items; i++) {
        bthread_mutex_lock(&cond_mutex);
        while (buffer_idx == 0) {  // Buffer empty
            bthread_cond_wait(&test_cond, &cond_mutex);
        }
        buffer_idx--;
        consumed++;
        bthread_cond_signal(&test_cond);
        bthread_mutex_unlock(&cond_mutex);
        bthread_yield();
    }
    return NULL;
}

void test_producer_consumer_cond() {
    TEST("Producer-consumer with condition variable");
    #define NUM_ITEMS 20
    
    bthread_cond_init(&test_cond, NULL);
    bthread_mutex_init(&cond_mutex, NULL);
    buffer_idx = 0;
    produced = 0;
    consumed = 0;
    
    bthread_t producer, consumer;
    int items = NUM_ITEMS;
    
    bthread_create(&producer, NULL, producer_cond_thread, &items);
    bthread_create(&consumer, NULL, consumer_cond_thread, &items);
    
    bthread_join(producer, NULL);
    bthread_join(consumer, NULL);
    
    ASSERT_TEST(produced == NUM_ITEMS, "Not all items produced");
    ASSERT_TEST(consumed == NUM_ITEMS, "Not all items consumed");
    ASSERT_TEST(buffer_idx == 0, "Buffer should be empty");
    
    bthread_cond_destroy(&test_cond);
    bthread_mutex_destroy(&cond_mutex);
    PASS();
}

/* ==================== MULTIPLE CONDITION VARIABLES ==================== */

bthread_cond_t cond1, cond2;
volatile int flag1 = 0, flag2 = 0;

void *multi_cond_thread(void *arg) {
    int id = *(int *)arg;
    
    if (id == 1) {
        bthread_mutex_lock(&cond_mutex);
        while (flag1 == 0) {
            bthread_cond_wait(&cond1, &cond_mutex);
        }
        bthread_mutex_unlock(&cond_mutex);
    } else {
        bthread_mutex_lock(&cond_mutex);
        while (flag2 == 0) {
            bthread_cond_wait(&cond2, &cond_mutex);
        }
        bthread_mutex_unlock(&cond_mutex);
    }
    
    return (void *)(long)id;
}

void test_multiple_conditions() {
    TEST("Multiple condition variables");
    
    bthread_cond_init(&cond1, NULL);
    bthread_cond_init(&cond2, NULL);
    bthread_mutex_init(&cond_mutex, NULL);
    flag1 = 0;
    flag2 = 0;
    
    bthread_t t1, t2;
    int id1 = 1, id2 = 2;
    
    bthread_create(&t1, NULL, multi_cond_thread, &id1);
    bthread_create(&t2, NULL, multi_cond_thread, &id2);
    
    bthread_sleep(20);
    
    // Signal first condition
    bthread_mutex_lock(&cond_mutex);
    flag1 = 1;
    bthread_cond_signal(&cond1);
    bthread_mutex_unlock(&cond_mutex);
    
    bthread_sleep(20);
    
    // Signal second condition
    bthread_mutex_lock(&cond_mutex);
    flag2 = 1;
    bthread_cond_signal(&cond2);
    bthread_mutex_unlock(&cond_mutex);
    
    void *ret1, *ret2;
    bthread_join(t1, &ret1);
    bthread_join(t2, &ret2);
    
    ASSERT_TEST((long)ret1 == 1, "Thread 1 didn't complete correctly");
    ASSERT_TEST((long)ret2 == 2, "Thread 2 didn't complete correctly");
    
    bthread_cond_destroy(&cond1);
    bthread_cond_destroy(&cond2);
    bthread_mutex_destroy(&cond_mutex);
    PASS();
}

/* ==================== SPURIOUS WAKEUP HANDLING ==================== */

volatile int work_available = 0;

void *spurious_wait_thread(void *arg) {
    int *work_done = (int *)arg;
    
    bthread_mutex_lock(&cond_mutex);
    while (work_available == 0) {  // Must use while, not if
        bthread_cond_wait(&test_cond, &cond_mutex);
    }
    *work_done = work_available;
    work_available = 0;
    bthread_mutex_unlock(&cond_mutex);
    
    return NULL;
}

void test_spurious_wakeup() {
    TEST("Spurious wakeup handling");
    
    bthread_cond_init(&test_cond, NULL);
    bthread_mutex_init(&cond_mutex, NULL);
    work_available = 0;
    
    bthread_t t;
    int work_done = 0;
    
    bthread_create(&t, NULL, spurious_wait_thread, &work_done);
    
    bthread_sleep(30);
    
    bthread_mutex_lock(&cond_mutex);
    work_available = 42;
    bthread_cond_signal(&test_cond);
    bthread_mutex_unlock(&cond_mutex);
    
    bthread_join(t, NULL);
    
    ASSERT_TEST(work_done == 42, "Work wasn't processed correctly");
    
    bthread_cond_destroy(&test_cond);
    bthread_mutex_destroy(&cond_mutex);
    PASS();
}

/* ==================== STRESS TEST ==================== */

volatile int stress_counter = 0;

void *stress_cond_thread(void *arg) {
    int iterations = *(int *)arg;
    
    for (int i = 0; i < iterations; i++) {
        bthread_mutex_lock(&cond_mutex);
        while (stress_counter % 2 != *(int *)arg % 2) {
            bthread_cond_wait(&test_cond, &cond_mutex);
        }
        stress_counter++;
        bthread_cond_broadcast(&test_cond);
        bthread_mutex_unlock(&cond_mutex);
        if (i % 3 == 0) bthread_yield();
    }
    
    return NULL;
}

void test_condition_stress() {
    TEST("Condition variable stress test");
    #define STRESS_ITERATIONS 30
    
    bthread_cond_init(&test_cond, NULL);
    bthread_mutex_init(&cond_mutex, NULL);
    stress_counter = 0;
    
    bthread_t t1, t2;
    int iter = STRESS_ITERATIONS;
    
    bthread_create(&t1, NULL, stress_cond_thread, &iter);
    bthread_create(&t2, NULL, stress_cond_thread, &iter);
    
    bthread_join(t1, NULL);
    bthread_join(t2, NULL);
    
    ASSERT_TEST(stress_counter == STRESS_ITERATIONS * 2, 
                "Stress test counter incorrect");
    
    bthread_cond_destroy(&test_cond);
    bthread_mutex_destroy(&cond_mutex);
    PASS();
}

/* ==================== MAIN ==================== */

int main() {
    printf("\n");
    PRINT_TITLE("  TCondition Variable Tests  ");
    printf("\n");
    
    PRINT_TITLE(" Basic Operations ");
    test_condition_init();
    test_condition_destroy();
    
    PRINT_TITLE(" Signal Operations ");
    test_cond_signal();
    
    PRINT_TITLE(" Broadcast Operations ");
    test_cond_broadcast();
    
    PRINT_TITLE(" Producer-Consumer ");
    test_producer_consumer_cond();
    
    PRINT_TITLE(" Multiple Conditions ");
    test_multiple_conditions();
    
    PRINT_TITLE(" Spurious Wakeup ");
    test_spurious_wakeup();
    
    PRINT_TITLE(" Stress Test ");
    test_condition_stress();
    
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
