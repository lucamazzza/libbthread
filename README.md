<p align='center'>
  
<img src="https://capsule-render.vercel.app/api?type=venom&color=323b9d&height=200&section=header&text=libbthread&fontSize=90&fontColor=ffffff&animation=fadeIn&fontAlignY=35&desc=User%20level%20threads%20library&descAlignY=61&descAlign=50"/>

</p>

<p align='center'>

<img src="https://img.shields.io/badge/11-%23323b9d?style=for-the-badge&logo=c&logoColor=%23323b9d&labelColor=%23ffffff&color=%23323b9d"/>

</p>

What is it
----------

BThread is a lightweight user-level threading library written in C. It provides
a simple and efficient cooperative threading model with support for common
synchronization primitives including mutexes, semaphores, condition variables,
barriers, and thread-safe queues.

On what hardware does it run?
-----------------------------

The library runs on Linux, macOS, and Windows systems with x86_64 architecture.
It uses setjmp/longjmp for context switching and is designed to be portable
across POSIX-compliant systems.

Documentation
-------------

Core API:
```c
bthread_create()     // Create a new thread
bthread_join()       // Wait for a thread to terminate
bthread_yield()      // Yield execution to another thread
bthread_exit()       // Terminate the calling thread
bthread_sleep()      // Sleep for specified milliseconds
bthread_cancel()     // Request thread cancellation
bthread_testcancel() // Check for cancellation requests
```

Synchronization Primitives:

- `tmutex`: Mutual exclusion locks
- `tsemaphore`: Counting semaphores
- `tcondition`: Condition variables
- `tbarrier`: Thread barriers

For detailed API documentation, see the header files in the include/ directory.

Installation
------------

Building the library:

```sh
make              # Build default demo and shared library
make lib          # Build only the shared library
make demos        # Build all demo programs
make test         # Build and run tests
make clean        # Clean build artifacts
```

The shared library will be built in the lib/ directory:

- Linux:   `libbthread.so`
- macOS:   `libbthread.dylib`
- Windows: `bthread.dll`

To use the library in your project:

1. Include the header: `#include <bthread.h>`
2. Link against the shared library: `-lbthread`
3. Add the include path: `-I/path/to/libbthread/include`
4. Add the library path: `-L/path/to/libbthread/lib`

Examples:
See the demos/ directory for complete examples including:

- Producer-Consumer patterns
- Dining Philosophers problem
- Reader-Writer locks
- Sleeping Barber problem

If something goes wrong
-----------------------

Common issues:

1. Compilation errors:
   - Ensure you have GCC installed and updated
   - Check that all source files are present
   - Verify include paths are correct

2. Linking errors:
   - Make sure the library path is in your LD_LIBRARY_PATH (Linux)
     or DYLD_LIBRARY_PATH (macOS)
   - Verify the shared library was built successfully

3. Runtime issues:
   - Check that threads are properly synchronized
   - Ensure thread_yield() is called regularly in cooperative scheduling
   - Verify all mutexes/semaphores are properly initialized

For bugs and issues, please report them at the project repository.

Copyright
---------

License: MIT License

Authors:
- Luca Mazza (C) 2025

This software is provided "as is" without warranty of any kind. See the
LICENSE file for complete terms and conditions.
