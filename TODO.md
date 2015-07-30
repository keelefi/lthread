# General

## Documentation

  * All missing

## Testing

  * Investigate if helgrind could be used

## Compatibility with the C++11 standard template library

  * Check if `std::unique_lock<lthread::mutex>` is possible

# Features

## thread local storage

Required

  * Implementation

## futex

Required

  * Support for robust mutexes

## mutex

Required

  * Error-handling: Separate EDEADLK somehow so that it's easy for the user to act accordingly
  * Support for robust mutexes

Optional

  * x86 assembly implementation
  * ARMv8 assembly implementation using LDA/STL

## spinlock

Required

  * Implementation using C++11 atomics

## condition variable

Required

  * Implementation
  * Tests

## thread

Required

  * Implementation inspired by Chris Wellons' blog post
    [Raw Linux Threads via System Calls](http://nullprogram.com/blog/2015/05/15/)
    (May 15, 2015. Last retrieved: July 30, 2015)
