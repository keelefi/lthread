#include "mutex.hh"

#include "futex.hh"

namespace lthread
{

mutex::mutex() :
        val(0)
{
}

mutex::~mutex()
{
}

void mutex::lock()
{
    int c;
    if ((c = __sync_val_compare_and_swap(&val, LOCK_FREE, LOCK_TAKEN)) != 0)
    {
        if (c != LOCK_TAKEN_WAITERS)
        {
            c = __sync_lock_test_and_set(&val, LOCK_TAKEN_WAITERS);
        }

        while (c != 0)
        {
            futex_wait(&val);
            c = __sync_lock_test_and_set(&val, LOCK_TAKEN_WAITERS);
        }
    }
}

void mutex::unlock()
{
    if (__sync_fetch_and_sub(&val, 1) != LOCK_TAKEN)
    {
        val = LOCK_FREE;
        futex_wake_one(&val);
    }
}

} // namespace lthread
