#include "futex.hh"

#include <linux/futex.h>
#include <unistd.h>
#include <sys/syscall.h>

namespace lthread
{

/*
 * syscall futex structure:
 *
 * int futex(
 *      int *uaddr,
 *      int op,
 *      int val,
 *      const struct timespec *timeout,
 *      int *uaddr2,
 *      int val3);
 */

int futex_wait(int *uaddr)
{
    return syscall(
            SYS_futex,
            uaddr,
            FUTEX_WAIT,
            LOCK_TAKEN_WAITERS, 
            NULL,
            NULL,
            0);
}

static int futex_wake(int *uaddr, int val)
{
    return syscall(
            SYS_futex,
            uaddr,
            FUTEX_WAKE,
            val,
            NULL,
            NULL,
            0);
}

int futex_wake_one(int *uaddr)
{
    return futex_wake(uaddr, 1);
}

int futex_wake_many(int *uaddr, int val)
{
    return futex_wake(uaddr, val);
}

/* PI futexes */

int futex_wait_pi(int *uaddr)
{
    return syscall(
            SYS_futex,
            uaddr,
            FUTEX_LOCK_PI,
            0,
            NULL,
            NULL,
            0);
}

int futex_wake_pi(int *uaddr)
{
    return syscall(
            SYS_futex,
            uaddr,
            FUTEX_UNLOCK_PI,
            0,
            NULL,
            NULL,
            0);
}

} // namespace lthread
