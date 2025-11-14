#include <stdio.h>
#include <stdlib.h>

#include "bthread.h"
#include "bthread_private.h"
#include "tsemaphore.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

/* ==================== BASIC TESTS ==================== */

void test_semaphore_init() {
    TEST("Semaphore initialization");
    bthread_sem_t sem;
    int result = bthread_sem_init(&sem, 0, 1);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(sem.value == 1, "Expected initial value 1");
    ASSERT_TEST(sem.waiting_queue == NULL, "Expected empty queue");
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

void test_semaphore_init_large() {
    TEST("Semaphore initialization with large value");
    bthread_sem_t sem;
    int result = bthread_sem_init(&sem, 0, 100);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(sem.value == 100, "Expected initial value 100");
    bthread_sem_destroy(&sem);
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

/* ==================== POST TESTS ==================== */

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

void test_semaphore_multiple_posts() {
    TEST("Semaphore multiple posts");
    bthread_sem_t sem;
    bthread_sem_init(&sem, 0, 0);
    
    for (int i = 0; i < 5; i++) {
        bthread_sem_post(&sem);
    }
    
    ASSERT_TEST(sem.value == 5, "Expected value 5 after 5 posts");
    bthread_sem_destroy(&sem);
    PASS();
}

/* ==================== WAIT/POST WITH THREADS ==================== */

bthread_sem_t test_sem;

void *wait_post_thread(void *arg) {
    int *value = (int *)arg;
    bthread_sem_wait(&test_sem);
    *value = 42;
    return NULL;
}

void test_wait_then_post() {
    TEST("Semaphore wait then post");
    bthread_sem_init(&test_sem, 0, 0);
    
    bthread_t t;
    int value = 0;
    
    bthread_create(&t, NULL, wait_post_thread, &value);
    
    bthread_sleep(10);
    bthread_sem_post(&test_sem);
    
    bthread_join(t, NULL);
    
    ASSERT_TEST(value == 42, "Thread didn't execute after post");
    
    bthread_sem_destroy(&test_sem);
    PASS();
}

/* ==================== PRODUCER-CONSUMER TESTS ==================== */

volatile int buffer[10];
volatile int buffer_count = 0;
bthread_sem_t empty_slots;
bthread_sem_t filled_slots;

void *producer_thread(void *arg) {
    int items = *(int *)arg;
    for (int i = 0; i < items; i++) {
        bthread_sem_wait(&empty_slots);
        buffer[buffer_count++] = i;
        bthread_sem_post(&filled_slots);
        bthread_yield();
    }
    return NULL;
}

void *consumer_thread(void *arg) {
    int items = *(int *)arg;
    for (int i = 0; i < items; i++) {
        bthread_sem_wait(&filled_slots);
        buffer_count--;
        bthread_sem_post(&empty_slots);
        bthread_yield();
    }
    return NULL;
}

void test_producer_consumer() {
    TEST("Semaphore producer-consumer");
    #define BUFFER_SIZE 5
    #define NUM_ITEMS 10
    
    bthread_sem_init(&empty_slots, 0, BUFFER_SIZE);
    bthread_sem_init(&filled_slots, 0, 0);
    buffer_count = 0;
    
    bthread_t producer, consumer;
    int items = NUM_ITEMS;
    
    bthread_create(&producer, NULL, producer_thread, &items);
    bthread_create(&consumer, NULL, consumer_thread, &items);
    
    bthread_join(producer, NULL);
    bthread_join(consumer, NULL);
    
    ASSERT_TEST(buffer_count == 0, "Buffer should be empty");
    
    bthread_sem_destroy(&empty_slots);
    bthread_sem_destroy(&filled_slots);
    PASS();
}

/* ==================== MULTIPLE WAITERS TEST ==================== */

volatile int sem_counter = 0;

void *multiple_waiter_thread(void *arg) {
    (void)arg;
    bthread_sem_wait(&test_sem);
    sem_counter++;
    bthread_sleep(20);
    sem_counter--;
    bthread_sem_post(&test_sem);
    return NULL;
}

void test_multiple_waiters() {
    TEST("Semaphore multiple waiters");
    #define MAX_CONCURRENT 2
    #define NUM_WAITERS 5
    
    bthread_sem_init(&test_sem, 0, MAX_CONCURRENT);
    sem_counter = 0;
    
    bthread_t threads[NUM_WAITERS];
    
    for (int i = 0; i < NUM_WAITERS; i++) {
        bthread_create(&threads[i], NULL, multiple_waiter_thread, NULL);
    }
    
    for (int i = 0; i < NUM_WAITERS; i++) {
        bthread_join(threads[i], NULL);
    }
    
    ASSERT_TEST(sem_counter == 0, "Counter should be 0 at end");
    
    bthread_sem_destroy(&test_sem);
    PASS();
}

/* ==================== BINARY SEMAPHORE (MUTEX-LIKE) TEST ==================== */

volatile int protected_value = 0;

void *binary_sem_thread(void *arg) {
    int iterations = *(int *)arg;
    for (int i = 0; i < iterations; i++) {
        bthread_sem_wait(&test_sem);
        protected_value++;
        bthread_yield();
        bthread_sem_post(&test_sem);
    }
    return NULL;
}

void test_binary_semaphore() {
    TEST("Binary semaphore (mutex-like)");
    #define NUM_BIN_THREADS 3
    #define BIN_ITERATIONS 30
    
    bthread_sem_init(&test_sem, 0, 1);  // Binary semaphore
    protected_value = 0;
    
    bthread_t threads[NUM_BIN_THREADS];
    int iterations = BIN_ITERATIONS;
    
    for (int i = 0; i < NUM_BIN_THREADS; i++) {
        bthread_create(&threads[i], NULL, binary_sem_thread, &iterations);
    }
    
    for (int i = 0; i < NUM_BIN_THREADS; i++) {
        bthread_join(threads[i], NULL);
    }
    
    ASSERT_TEST(protected_value == NUM_BIN_THREADS * BIN_ITERATIONS,
                "Binary semaphore mutual exclusion failed");
    
    bthread_sem_destroy(&test_sem);
    PASS();
}

/* ==================== COUNTING SEMAPHORE TEST ==================== */

volatile int active_count = 0;
volatile int max_observed = 0;

void *counting_sem_thread(void *arg) {
    (void)arg;
    bthread_sem_wait(&test_sem);
    
    active_count++;
    if (active_count > max_observed) {
        max_observed = active_count;
    }
    
    bthread_sleep(10);
    
    active_count--;
    bthread_sem_post(&test_sem);
    
    return NULL;
}

void test_counting_semaphore() {
    TEST("Counting semaphore limits");
    #define MAX_ACTIVE 3
    #define NUM_COUNT_THREADS 8
    
    bthread_sem_init(&test_sem, 0, MAX_ACTIVE);
    active_count = 0;
    max_observed = 0;
    
    bthread_t threads[NUM_COUNT_THREADS];
    
    for (int i = 0; i < NUM_COUNT_THREADS; i++) {
        bthread_create(&threads[i], NULL, counting_sem_thread, NULL);
    }
    
    for (int i = 0; i < NUM_COUNT_THREADS; i++) {
        bthread_join(threads[i], NULL);
    }
    
    ASSERT_TEST(max_observed <= MAX_ACTIVE, 
                "Semaphore allowed too many concurrent threads");
    ASSERT_TEST(active_count == 0, "Not all threads finished");
    
    bthread_sem_destroy(&test_sem);
    PASS();
}

/* ==================== STRESS TEST ==================== */

void *stress_sem_thread(void *arg) {
    volatile int *counter = (int *)arg;
    for (int i = 0; i < 20; i++) {
        bthread_sem_wait(&test_sem);
        (*counter)++;
        if (i % 5 == 0) bthread_yield();
        bthread_sem_post(&test_sem);
        if (i % 3 == 0) bthread_sleep(5);
    }
    return NULL;
}

void test_semaphore_stress() {
    TEST("Semaphore stress test");
    #define STRESS_SEM_THREADS 6
    
    bthread_sem_init(&test_sem, 0, 1);
    
    bthread_t threads[STRESS_SEM_THREADS];
    volatile int counter = 0;
    
    for (int i = 0; i < STRESS_SEM_THREADS; i++) {
        bthread_create(&threads[i], NULL, stress_sem_thread, (void *)&counter);
    }
    
    for (int i = 0; i < STRESS_SEM_THREADS; i++) {
        bthread_join(threads[i], NULL);
    }
    
    ASSERT_TEST(counter == STRESS_SEM_THREADS * 20, "Stress test failed");
    
    bthread_sem_destroy(&test_sem);
    PASS();
}

/* ==================== MAIN ==================== */

int main() {
    printf("\n");
    PRINT_TITLE("  TSemaphore Tests  ");
    printf("\n");
    
    PRINT_TITLE(" Basic Operations ");
    test_semaphore_init();
    test_semaphore_init_zero();
    test_semaphore_init_large();
    test_semaphore_destroy();
    
    PRINT_TITLE(" Post Operations ");
    test_semaphore_post();
    test_semaphore_multiple_posts();
    
    PRINT_TITLE(" Wait/Post with Threads ");
    test_wait_then_post();
    
    PRINT_TITLE(" Producer-Consumer ");
    test_producer_consumer();
    
    PRINT_TITLE(" Multiple Waiters ");
    test_multiple_waiters();
    
    PRINT_TITLE(" Binary Semaphore ");
    test_binary_semaphore();
    
    PRINT_TITLE(" Counting Semaphore ");
    test_counting_semaphore();
    
    PRINT_TITLE(" Stress Test ");
    test_semaphore_stress();
    
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
