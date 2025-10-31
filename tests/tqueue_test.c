#include <stdio.h>
#include <stdlib.h>

#include "tqueue.h"
#include "test.h"

static int test_count = 0;
static int test_passed = 0;

void test_queue_init() {
    TEST("Queue initialization");
    tqueue_t q = NULL;
    ASSERT_TEST(tqueue_size(q) == 0, "Expected size 0");
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
    ASSERT_TEST(tqueue_size(q) == 1, "Expected size 1");
    PASS();
}

int main() {
    PRINT_TITLE("Running tqueue Tests");
    test_queue_init();
    test_queue_enqueue();
    test_queue_pop();
    test_queue_pop_empty();
    test_queue_get_data();
    PRINT_TITLE("Test Results");
    printf("Tests run:    %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    return (test_count == test_passed) ? 0 : 1;
}
