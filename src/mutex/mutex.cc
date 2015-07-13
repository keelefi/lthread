#include "mutex.hh"

#include "exception.hh"
#include "futex.hh"

#include <sstream>

#include <errno.h>
#include <string.h>

namespace lthread
{

mutex::mutex() :
        val(LOCK_FREE)
{
}

mutex::~mutex()
{
}

void mutex::lock()
{
    int c;
    if ((c = __sync_val_compare_and_swap(&val, LOCK_FREE, LOCK_TAKEN))
            != LOCK_FREE)
    {
        if (c != LOCK_TAKEN_WAITERS)
        {
            c = __sync_lock_test_and_set(&val, LOCK_TAKEN_WAITERS);
        }

        while (c != LOCK_FREE)
        {
            int futex_return_value = futex_wait(&val);
            if (futex_return_value == -1)
            {
                // TODO: strerror() is not thread-safe. But simply replacing it
                // with strerror_r() is not what we want. Come up with a better
                // idea. Maybe use C++11 STL to print the error?
                std::ostringstream ostream;
                ostream << "futex_wait() failed with "
                        << errno
                        << " - "
                        << strerror(errno);

                throw lthread::exception(ostream.str());
            }

            c = __sync_lock_test_and_set(&val, LOCK_TAKEN_WAITERS);
        }
    }
}

void mutex::unlock()
{
    if (__sync_fetch_and_sub(&val, 1) != LOCK_TAKEN)
    {
        val = LOCK_FREE;

        if (futex_wake_one(&val) == -1)
        {
            // TODO: strerror() is not thread-safe. But simply replacing it
            // with strerror_r() is not what we want. Come up with a better
            // idea. Maybe use C++11 STL to print the error?
            std::ostringstream ostream;
            ostream << "futex_wake() failed with"
                    << errno
                    << " - "
                    << strerror(errno);

            throw lthread::exception(ostream.str());
        }
    }
}

} // namespace lthread
