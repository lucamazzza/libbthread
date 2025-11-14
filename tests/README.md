# LibBThread Test Suite

Comprehensive test suite for the LibBThread user-level threading library.

## Test Files

- **bthread_test.c**: Core thread operations (create, join, yield, exit, sleep, cancellation)
- **tmutex_test.c**: Mutex operations (lock, unlock, trylock, mutual exclusion)
- **tsemaphore_test.c**: Semaphore operations (wait, post, counting, binary semaphores)
- **tbarrier_test.c**: Barrier synchronization (single/multiple phases, ordered execution)
- **tcondition_test.c**: Condition variables (signal, broadcast, producer-consumer)
- **tqueue_test.c**: Queue operations (circular queue implementation)

## Running Tests

### Run All Tests

```bash
./run_all_tests.sh
```

### Run Individual Test

```bash
# Compile
gcc -std=gnu11 -g -Wall -Wextra -Iinclude \
    tests/bthread_test.c obj/*.o -o tests/bthread_test -lm

# Run
./tests/bthread_test
```

## Test Coverage

### BThread Core (bthread_test.c)
- ✓ Thread creation with/without attributes
- ✓ Thread join with/without return value
- ✓ Multiple concurrent threads
- ✓ Thread yielding
- ✓ Thread exit (normal and immediate)
- ✓ Thread sleep (single and multiple)
- ✓ Thread cancellation
- ✓ Cancellation points (testcancel)
- ✓ Edge cases (no yield, return NULL)
- ✓ Stress test with many threads

### Mutex (tmutex_test.c)
- ✓ Mutex initialization and destruction
- ✓ Lock and unlock operations
- ✓ Mutual exclusion verification
- ✓ Trylock (success and failure)
- ✓ Blocking behavior
- ✓ Nested locks
- ✓ Stress test

### Semaphore (tsemaphore_test.c)
- ✓ Semaphore initialization (zero, one, large values)
- ✓ Post operations (single and multiple)
- ✓ Wait and post with threads
- ✓ Producer-consumer pattern
- ✓ Multiple concurrent waiters
- ✓ Binary semaphore (mutex-like)
- ✓ Counting semaphore limits
- ✓ Stress test

### Barrier (tbarrier_test.c)
- ✓ Barrier initialization and destruction
- ✓ Basic synchronization
- ✓ Multiple phase synchronization
- ✓ Ordered execution verification
- ✓ Single thread barrier
- ✓ Stress test

### Condition Variable (tcondition_test.c)
- ✓ Condition variable initialization and destruction
- ✓ Signal operation
- ✓ Broadcast operation
- ✓ Producer-consumer with condition variables
- ✓ Multiple condition variables
- ✓ Spurious wakeup handling
- ✓ Stress test

### Queue (tqueue_test.c)
- ✓ Queue initialization
- ✓ Enqueue and dequeue operations
- ✓ Get data without removal
- ✓ At offset operations (with wraparound)
- ✓ Circular behavior verification
- ✓ Multiple operations
- ✓ Single element handling
- ✓ Pointer integrity
- ✓ Interleaved operations
- ✓ Stress test with 100 items

## Test Statistics

Total test functions: **80+**
Test categories: **6 modules**
Coverage: **All public APIs and edge cases**

## Test Output

Each test displays:
- Test name
- Pass/Fail status with color coding (green ✓ / red ✗)
- Failure reason (if applicable)
- Summary statistics

Example output:
```
═══════════════ Basic Operations ═══════════════
Running test: Thread creation...................PASSED
Running test: Thread creation with NULL attr....PASSED
...

═══════════════ Test Summary ═══════════════
Tests run:    25
Tests passed: 25
Tests failed: 0

✓ All tests passed!
```

## Adding New Tests

1. Create test function following the pattern:
```c
void test_my_feature() {
    TEST("Description of test");
    
    // Test code here
    ASSERT_TEST(condition, "Error message");
    
    PASS();
}
```

2. Add to main():
```c
int main() {
    // ...
    test_my_feature();
    // ...
}
```

3. Add to `run_all_tests.sh` if creating a new test file.

## Requirements

- GCC with C11 support
- LibBThread library (compiled obj files)
- POSIX-compliant system (Linux, macOS)

## Notes

- Tests use preemption (timer-based scheduling)
- Some tests include intentional delays for timing verification
- Stress tests may take longer to complete
- All tests should be deterministic (no race conditions)
