# General

## Documentation

  * All missing

## Testing

  * Figure out a way to run all tests at once
  * Run tests with helgrind
  * Write usage instructions to run tests using valgrind/helgrind

## Continuous integration

  * Run tests for each commit before accepting it to master (maybe Travis CI?)

## Compatibility with the C++11 standard template library

  * Check if `std::unique_lock<lthread::mutex>` is possible

# Features

## futex

Required

  * Support for priority inheritance mutexes
  * Support for robust mutexes

## mutex

Required

  * Error-handling: Separate EDEADLK somehow so that it's easy for the user to act accordingly
  * Support for priority inheritance mutexes
  * Support for robust mutexes
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

  * Implementation inspired by [Chris Wellons' blog post](http://nullprogram.com/blog/2015/05/15/)
