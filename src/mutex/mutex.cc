#include "mutex.hh"

#include "exception.hh"
#include "futex.hh"
#include "gettid.hh"

#include <sstream>
#include <system_error>

#include <linux/futex.h>


namespace lthread
{

mutex::mutex() :
        m_val(LOCK_FREE),
        m_pi(false)
{
}

mutex::mutex(bool pi) :
        m_val(LOCK_FREE),
        m_pi(pi)
{
}

mutex::~mutex()
{
}

void mutex::lock()
{
    if (m_pi)
    {
        lock_pi();
    }
    else
    {
        lock_nonpi();
    }
}

void mutex::unlock()
{
    if (m_pi)
    {
        unlock_pi();
    }
    else
    {
        unlock_nonpi();
    }
}

void mutex::lock_pi()
{
    // TODO: Defeats the purpose if we make a system call here every time.
    // Learn how to store the thread ID without using the pthread library.
    int thread_id = gettid();

    int oldval;
    while ((oldval = __sync_val_compare_and_swap(&m_val, LOCK_FREE, thread_id))
            != LOCK_FREE)
    {
        int futex_return_value = futex_wait_pi(&m_val);
        if (futex_return_value == -1)
        {
            if (errno == EAGAIN)
            {
                continue ;
            }

            std::error_code e(errno, std::system_category());

            std::ostringstream ostream;
            ostream << "futex_wait() failed with: "
                    << e.value()
                    << " - "
                    << e.message();

            throw lthread::exception(ostream.str());
        }

        if ((m_val & FUTEX_TID_MASK) == thread_id)
        {
            break ;
        }
    }
}

void mutex::unlock_pi()
{
    // TODO: Defeats the purpose if we make a system call here every time.
    int thread_id = gettid();

    if (((m_val & FUTEX_WAITERS) != 0) ||
            (!__sync_bool_compare_and_swap(&m_val, thread_id, 0)))
    {
        if (futex_wake_pi(&m_val) == -1)
        {
            std::error_code e(errno, std::system_category());

            std::ostringstream ostream;
            ostream << "futex_wake() failed with: "
                    << e.value()
                    << " - "
                    << e.message();

            throw lthread::exception(ostream.str());
        }
    }
}

void mutex::lock_nonpi()
{
    int c;
    while ((c = __sync_val_compare_and_swap(&m_val, LOCK_FREE, LOCK_TAKEN))
            != LOCK_FREE)
    {
        if (c != LOCK_TAKEN_WAITERS)
        {
            c = __sync_lock_test_and_set(&m_val, LOCK_TAKEN_WAITERS);
        }

        while (c != LOCK_FREE)
        {
            int futex_return_value = futex_wait(&m_val);
            if (futex_return_value == -1)
            {
                if (errno == EAGAIN)
                {
                    break ;
                }

                std::error_code e(errno, std::system_category());

                std::ostringstream ostream;
                ostream << "futex_wait() failed with: "
                        << e.value()
                        << " - "
                        << e.message();

                throw lthread::exception(ostream.str());
            }

            c = __sync_lock_test_and_set(&m_val, LOCK_TAKEN_WAITERS);
        }

        if (c == LOCK_FREE)
        {
            break ;
        }
    }
}

void mutex::unlock_nonpi()
{
    if (__sync_fetch_and_sub(&m_val, 1) != LOCK_TAKEN)
    {
        m_val = LOCK_FREE;

        if (futex_wake_one(&m_val) == -1)
        {
            std::error_code e(errno, std::system_category());

            std::ostringstream ostream;
            ostream << "futex_wake() failed with: "
                    << e.value()
                    << " - "
                    << e.message();

            throw lthread::exception(ostream.str());
        }
    }
}

} // namespace lthread
