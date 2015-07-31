# Thread Local Storage

Implementing the Thread Local Storage (TLS) will actually be somewhat tricky.
There are several things to take care of:

  * Out of the compilers at least GCC supports a keyword `__thread` that will
    create thread local variables. Probably our implementation should support
    that too?
  * errno is always a thread local variable.

The thread local storage will be set into a special area of our thread stack.
For each thread we need to allocate memory using `mmap(2)`. This area will be
contain:

  * The normal stack frames.
  * A guard area set with `mprotect(2)` to `PROT_NONE` so that if the stack
    grows over we get a `SIGSEGV` if our stack overflows.
  * Thread local storage area.
