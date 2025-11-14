#include <stdio.h>
#include <stdlib.h>

#include "tqueue.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

/* ==================== BASIC TESTS ==================== */

void test_queue_init() {
    TEST("Queue initialization");
    tqueue_t q = NULL;
    ASSERT_TEST(tqueue_size(q) == 0, "Expected size 0");
    ASSERT_TEST(tqueue_get_data(q) == NULL, "Expected NULL data");
    PASS();
}

void test_queue_enqueue() {
    TEST("Queue enqueue");
    tqueue_t q = NULL;
    int val1 = 42, val2 = 100;
    tqueue_enqueue(&q, &val1);
    ASSERT_TEST(tqueue_size(q) == 1, "Expected size 1");
    tqueue_enqueue(&q, &val2);
    ASSERT_TEST(tqueue_size(q) == 2, "Expected size 2");
    PASS();
}

void test_queue_pop() {
    TEST("Queue pop");
    tqueue_t q = NULL;
    int val1 = 42, val2 = 100;
    tqueue_enqueue(&q, &val1);
    tqueue_enqueue(&q, &val2);
    int *result = (int *)tqueue_pop(&q);
    ASSERT_TEST(result != NULL, "Expected non-NULL result");
    ASSERT_TEST(*result == 42, "Expected value 42");
    ASSERT_TEST(tqueue_size(q) == 1, "Expected size 1");
    result = (int *)tqueue_pop(&q);
    ASSERT_TEST(result != NULL, "Expected non-NULL result");
    ASSERT_TEST(*result == 100, "Expected value 100");
    ASSERT_TEST(tqueue_size(q) == 0, "Expected size 0");
    PASS();
}

void test_queue_pop_empty() {
    TEST("Queue pop empty");
    tqueue_t q = NULL;
    void *result = tqueue_pop(&q);
    ASSERT_TEST(result == NULL, "Expected NULL result");
    ASSERT_TEST(tqueue_size(q) == 0, "Size should remain 0");
    PASS();
}

void test_queue_get_data() {
    TEST("Queue get_data");
    tqueue_t q = NULL;
    int val = 42;
    tqueue_enqueue(&q, &val);
    int *result = (int *)tqueue_get_data(q);
    ASSERT_TEST(result != NULL, "Expected non-NULL result");
    ASSERT_TEST(*result == 42, "Expected value 42");
    ASSERT_TEST(tqueue_size(q) == 1, "Size should remain 1");
    PASS();
}

/* ==================== AT_OFFSET TESTS ==================== */

void test_queue_at_offset() {
    TEST("Queue at_offset");
    tqueue_t q = NULL;
    int vals[5] = {10, 20, 30, 40, 50};
    
    for (int i = 0; i < 5; i++) {
        tqueue_enqueue(&q, &vals[i]);
    }
    
    tqueue_t node = tqueue_at_offset(q, 0);
    ASSERT_TEST(*(int *)tqueue_get_data(node) == 10, "Expected value 10 at offset 0");
    
    node = tqueue_at_offset(q, 2);
    ASSERT_TEST(*(int *)tqueue_get_data(node) == 30, "Expected value 30 at offset 2");
    
    node = tqueue_at_offset(q, 4);
    ASSERT_TEST(*(int *)tqueue_get_data(node) == 50, "Expected value 50 at offset 4");
    
    PASS();
}

void test_queue_at_offset_wraparound() {
    TEST("Queue at_offset with wraparound");
    tqueue_t q = NULL;
    int vals[3] = {10, 20, 30};
    
    for (int i = 0; i < 3; i++) {
        tqueue_enqueue(&q, &vals[i]);
    }
    
    // Wraparound: offset 3 should give us first element
    tqueue_t node = tqueue_at_offset(q, 3);
    ASSERT_TEST(*(int *)tqueue_get_data(node) == 10, "Expected wraparound to first");
    
    // offset 5 should give us third element (5 % 3 = 2)
    node = tqueue_at_offset(q, 5);
    ASSERT_TEST(*(int *)tqueue_get_data(node) == 30, "Expected wraparound to third");
    
    PASS();
}

void test_queue_at_offset_null() {
    TEST("Queue at_offset on empty queue");
    tqueue_t q = NULL;
    tqueue_t node = tqueue_at_offset(q, 0);
    ASSERT_TEST(node == NULL, "Expected NULL for empty queue");
    PASS();
}

/* ==================== CIRCULAR BEHAVIOR TESTS ==================== */

void test_queue_circular() {
    TEST("Queue circular behavior");
    tqueue_t q = NULL;
    int vals[3] = {1, 2, 3};
    
    for (int i = 0; i < 3; i++) {
        tqueue_enqueue(&q, &vals[i]);
    }
    
    // Traverse the circular queue
    tqueue_t node = q;
    for (int i = 0; i < 6; i++) {  // Go around twice
        int expected = (i % 3) + 1;
        ASSERT_TEST(*(int *)tqueue_get_data(node) == expected, 
                   "Circular traversal failed");
        node = tqueue_at_offset(node, 1);
    }
    
    PASS();
}

/* ==================== MULTIPLE ENQUEUE/DEQUEUE TESTS ==================== */

void test_queue_many_operations() {
    TEST("Queue many enqueue/dequeue operations");
    tqueue_t q = NULL;
    
    // Enqueue 10 items
    int vals[10];
    for (int i = 0; i < 10; i++) {
        vals[i] = i * 10;
        tqueue_enqueue(&q, &vals[i]);
    }
    
    ASSERT_TEST(tqueue_size(q) == 10, "Expected size 10");
    
    // Dequeue 5 items
    for (int i = 0; i < 5; i++) {
        int *val = (int *)tqueue_pop(&q);
        ASSERT_TEST(*val == i * 10, "Dequeued wrong value");
    }
    
    ASSERT_TEST(tqueue_size(q) == 5, "Expected size 5");
    
    // Enqueue 3 more
    int more_vals[3] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        tqueue_enqueue(&q, &more_vals[i]);
    }
    
    ASSERT_TEST(tqueue_size(q) == 8, "Expected size 8");
    
    // Dequeue all
    for (int i = 0; i < 8; i++) {
        tqueue_pop(&q);
    }
    
    ASSERT_TEST(tqueue_size(q) == 0, "Expected empty queue");
    ASSERT_TEST(q == NULL, "Queue should be NULL when empty");
    
    PASS();
}

/* ==================== SINGLE ELEMENT TESTS ==================== */

void test_queue_single_element() {
    TEST("Queue with single element");
    tqueue_t q = NULL;
    int val = 99;
    
    tqueue_enqueue(&q, &val);
    ASSERT_TEST(tqueue_size(q) == 1, "Expected size 1");
    
    int *result = (int *)tqueue_get_data(q);
    ASSERT_TEST(*result == 99, "Expected value 99");
    
    result = (int *)tqueue_pop(&q);
    ASSERT_TEST(*result == 99, "Expected popped value 99");
    ASSERT_TEST(q == NULL, "Queue should be NULL after pop");
    ASSERT_TEST(tqueue_size(q) == 0, "Expected size 0");
    
    PASS();
}

/* ==================== POINTER INTEGRITY TESTS ==================== */

void test_queue_pointer_integrity() {
    TEST("Queue pointer integrity");
    tqueue_t q = NULL;
    int vals[5] = {1, 2, 3, 4, 5};
    void *ptrs[5];
    
    // Store pointers
    for (int i = 0; i < 5; i++) {
        tqueue_enqueue(&q, &vals[i]);
        ptrs[i] = &vals[i];
    }
    
    // Verify pointers are preserved
    tqueue_t node = q;
    for (int i = 0; i < 5; i++) {
        void *data = tqueue_get_data(node);
        ASSERT_TEST(data == ptrs[i], "Pointer not preserved");
        node = tqueue_at_offset(node, 1);
    }
    
    PASS();
}

/* ==================== STRESS TEST ==================== */

void test_queue_stress() {
    TEST("Queue stress test");
    tqueue_t q = NULL;
    
    #define STRESS_SIZE 100
    int *vals = malloc(STRESS_SIZE * sizeof(int));
    
    // Enqueue many items
    for (int i = 0; i < STRESS_SIZE; i++) {
        vals[i] = i;
        tqueue_enqueue(&q, &vals[i]);
    }
    
    ASSERT_TEST(tqueue_size(q) == STRESS_SIZE, "Size incorrect after bulk enqueue");
    
    // Verify all items
    tqueue_t node = q;
    for (int i = 0; i < STRESS_SIZE; i++) {
        ASSERT_TEST(*(int *)tqueue_get_data(node) == i, "Value incorrect");
        node = tqueue_at_offset(node, 1);
    }
    
    // Dequeue all items
    for (int i = 0; i < STRESS_SIZE; i++) {
        int *val = (int *)tqueue_pop(&q);
        ASSERT_TEST(*val == i, "Dequeued value incorrect");
    }
    
    ASSERT_TEST(q == NULL, "Queue should be empty");
    
    free(vals);
    PASS();
}

/* ==================== INTERLEAVED OPERATIONS TEST ==================== */

void test_queue_interleaved() {
    TEST("Queue interleaved enqueue/dequeue");
    tqueue_t q = NULL;
    int vals[20];
    
    for (int i = 0; i < 20; i++) {
        vals[i] = i;
    }
    
    // Add 5
    for (int i = 0; i < 5; i++) {
        tqueue_enqueue(&q, &vals[i]);
    }
    
    // Remove 2
    for (int i = 0; i < 2; i++) {
        tqueue_pop(&q);
    }
    
    ASSERT_TEST(tqueue_size(q) == 3, "Size should be 3");
    
    // Add 3 more
    for (int i = 5; i < 8; i++) {
        tqueue_enqueue(&q, &vals[i]);
    }
    
    ASSERT_TEST(tqueue_size(q) == 6, "Size should be 6");
    
    // Remove all
    while (tqueue_size(q) > 0) {
        tqueue_pop(&q);
    }
    
    ASSERT_TEST(q == NULL, "Queue should be empty");
    
    PASS();
}

/* ==================== MAIN ==================== */

int main() {
    printf("\n");
    PRINT_TITLE("  TQueue Tests  ");
    printf("\n");
    
    PRINT_TITLE(" Basic Operations ");
    test_queue_init();
    test_queue_enqueue();
    test_queue_pop();
    test_queue_pop_empty();
    test_queue_get_data();
    
    PRINT_TITLE(" At Offset Operations ");
    test_queue_at_offset();
    test_queue_at_offset_wraparound();
    test_queue_at_offset_null();
    
    PRINT_TITLE(" Circular Behavior ");
    test_queue_circular();
    
    PRINT_TITLE(" Multiple Operations ");
    test_queue_many_operations();
    
    PRINT_TITLE(" Single Element ");
    test_queue_single_element();
    
    PRINT_TITLE(" Pointer Integrity ");
    test_queue_pointer_integrity();
    
    PRINT_TITLE(" Interleaved Operations ");
    test_queue_interleaved();
    
    PRINT_TITLE(" Stress Test ");
    test_queue_stress();
    
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
