#include <stdio.h>
#include <stdlib.h>

#include "bthread.h"
#include "tbarrier.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

/* ==================== BASIC TESTS ==================== */

void test_barrier_init() {
    TEST("Barrier initialization");
    bthread_barrier_t barrier;
    int result = bthread_barrier_init(&barrier, NULL, 3);
    ASSERT_TEST(result == 0, "Expected result 0");
    ASSERT_TEST(barrier.count == 3, "Expected count 3");
    ASSERT_TEST(barrier.size == 0, "Expected size 0");
    ASSERT_TEST(barrier.waiting_queue == NULL, "Expected empty queue");
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

/* ==================== SYNCHRONIZATION TESTS ==================== */

bthread_barrier_t test_barrier;
volatile int barrier_stage[10] = {0};

void *barrier_thread(void *arg) {
    int id = *(int *)arg;
    
    barrier_stage[id] = 1;
    bthread_barrier_wait(&test_barrier);
    barrier_stage[id] = 2;
    
    return NULL;
}

void test_barrier_sync() {
    TEST("Barrier synchronization");
    #define NUM_THREADS 5
    
    bthread_barrier_init(&test_barrier, NULL, NUM_THREADS);
    
    bthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; i++) {
        barrier_stage[i] = 0;
        ids[i] = i;
        bthread_create(&threads[i], NULL, barrier_thread, &ids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        bthread_join(threads[i], NULL);
    }
    
    // All threads should have reached stage 2
    for (int i = 0; i < NUM_THREADS; i++) {
        ASSERT_TEST(barrier_stage[i] == 2, "Thread didn't pass barrier");
    }
    
    bthread_barrier_destroy(&test_barrier);
    PASS();
}

/* ==================== MULTIPLE PHASES TEST ==================== */

void *multi_phase_barrier_thread(void *arg) {
    int id = *(int *)arg;
    
    for (int phase = 0; phase < 3; phase++) {
        barrier_stage[id] = phase;
        bthread_barrier_wait(&test_barrier);
        bthread_yield();
    }
    barrier_stage[id] = 99; // Done marker
    
    return NULL;
}

void test_barrier_multiple_phases() {
    TEST("Barrier multiple synchronizations");
    #define NUM_MULTI 4
    
    bthread_barrier_init(&test_barrier, NULL, NUM_MULTI);
    
    bthread_t threads[NUM_MULTI];
    int ids[NUM_MULTI];
    
    for (int i = 0; i < NUM_MULTI; i++) {
        ids[i] = i;
        bthread_create(&threads[i], NULL, multi_phase_barrier_thread, &ids[i]);
    }
    
    for (int i = 0; i < NUM_MULTI; i++) {
        bthread_join(threads[i], NULL);
    }
    
    // All should have completed all phases
    for (int i = 0; i < NUM_MULTI; i++) {
        ASSERT_TEST(barrier_stage[i] == 99, "Thread didn't complete all phases");
    }
    
    bthread_barrier_destroy(&test_barrier);
    PASS();
}

/* ==================== ORDERED EXECUTION TEST ==================== */

volatile int execution_order[20];
volatile int order_idx = 0;

void *ordered_barrier_thread(void *arg) {
    int id = *(int *)arg;
    
    // Phase 1
    execution_order[order_idx++] = id * 10 + 1;
    bthread_barrier_wait(&test_barrier);
    
    // Phase 2 - all threads should be synchronized here
    execution_order[order_idx++] = id * 10 + 2;
    bthread_barrier_wait(&test_barrier);
    
    return NULL;
}

void test_barrier_ordered() {
    TEST("Barrier ordered execution");
    #define NUM_ORDERED 3
    
    bthread_barrier_init(&test_barrier, NULL, NUM_ORDERED);
    order_idx = 0;
    
    bthread_t threads[NUM_ORDERED];
    int ids[NUM_ORDERED];
    
    for (int i = 0; i < NUM_ORDERED; i++) {
        ids[i] = i;
        bthread_create(&threads[i], NULL, ordered_barrier_thread, &ids[i]);
    }
    
    for (int i = 0; i < NUM_ORDERED; i++) {
        bthread_join(threads[i], NULL);
    }
    
    // Verify all phase 1 executions happened before phase 2
    int phase1_max = 0, phase2_min = 100;
    for (int i = 0; i < order_idx; i++) {
        int val = execution_order[i];
        if (val % 10 == 1) {
            if (val > phase1_max) phase1_max = val;
        } else if (val % 10 == 2) {
            if (val < phase2_min) phase2_min = val;
        }
    }
    
    ASSERT_TEST(phase1_max < phase2_min, "Barrier didn't synchronize phases");
    
    bthread_barrier_destroy(&test_barrier);
    PASS();
}

/* ==================== SINGLE THREAD BARRIER TEST ==================== */

void *single_thread_barrier(void *arg) {
    int *value = (int *)arg;
    *value = 1;
    bthread_barrier_wait(&test_barrier);
    *value = 2;
    return NULL;
}

void test_barrier_single_thread() {
    TEST("Barrier with single thread");
    bthread_barrier_init(&test_barrier, NULL, 1);
    
    bthread_t t;
    int value = 0;
    
    bthread_create(&t, NULL, single_thread_barrier, &value);
    bthread_join(t, NULL);
    
    ASSERT_TEST(value == 2, "Single thread didn't pass barrier");
    
    bthread_barrier_destroy(&test_barrier);
    PASS();
}

/* ==================== STRESS TEST ==================== */

void *stress_barrier_thread(void *arg) {
    int id = *(int *)arg;
    
    for (int i = 0; i < 5; i++) {
        barrier_stage[id] = i;
        bthread_barrier_wait(&test_barrier);
        if (i % 2 == 0) bthread_yield();
    }
    
    return NULL;
}

void test_barrier_stress() {
    TEST("Barrier stress test");
    #define STRESS_THREADS 6
    
    bthread_barrier_init(&test_barrier, NULL, STRESS_THREADS);
    
    bthread_t threads[STRESS_THREADS];
    int ids[STRESS_THREADS];
    
    for (int i = 0; i < STRESS_THREADS; i++) {
        ids[i] = i;
        bthread_create(&threads[i], NULL, stress_barrier_thread, &ids[i]);
    }
    
    for (int i = 0; i < STRESS_THREADS; i++) {
        bthread_join(threads[i], NULL);
        ASSERT_TEST(barrier_stage[i] == 4, "Thread didn't complete all iterations");
    }
    
    bthread_barrier_destroy(&test_barrier);
    PASS();
}

/* ==================== MAIN ==================== */

int main() {
    printf("\n");
    PRINT_TITLE("  TBarrier Tests  ");
    printf("\n");
    
    PRINT_TITLE(" Basic Operations ");
    test_barrier_init();
    test_barrier_destroy();
    
    PRINT_TITLE(" Synchronization ");
    test_barrier_sync();
    
    PRINT_TITLE(" Multiple Phases ");
    test_barrier_multiple_phases();
    
    PRINT_TITLE(" Ordered Execution ");
    test_barrier_ordered();
    
    PRINT_TITLE(" Single Thread ");
    test_barrier_single_thread();
    
    PRINT_TITLE(" Stress Test ");
    test_barrier_stress();
    
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
