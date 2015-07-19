#include "mutex.hh"

#include "exception.hh"
#include "futex.hh"

#include <sstream>
#include <system_error>


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
}

void mutex::unlock_pi()
{
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
