#pragma once

namespace lthread
{

enum LOCK_STATE
{
    LOCK_FREE = 0,
    LOCK_TAKEN = 1,
    LOCK_TAKEN_WAITERS = 2
};

int futex_wait(int *uaddr);

int futex_wake_one(int *uaddr);

int futex_wake_many(int *uaddr, int val);

/* PI futexes */

int futex_wait_pi(int *uaddr);
int futex_wake_pi(int *uaddr);

} // namespace lthread
