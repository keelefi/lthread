#include "condition_variable.hh"

#include "futex.hh"

namespace lthread
{

condition_variable::condition_variable() :
        m_seq_no_waiter(0),
        m_seq_no_wakeup(0)
{
}

void condition_variable::signal(lthread::mutex& mutex)
{
    if (m_seq_no_waiter <= m_seq_no_wakeup)
    {
        // No waiters
        mutex.unlock();
        return ;
    }

    m_seq_no_wakeup++;

    mutex.unlock();

    futex_wake_one(&m_seq_no_wakeup);
}

void condition_variable::wait(lthread::mutex& mutex)
{
    int my_seq_no = ++m_seq_no_waiter;

    while (my_seq_no > m_seq_no_wakeup)
    {
        mutex.unlock();

        futex_wait(&m_seq_no_wakeup, m_seq_no_wakeup);

        mutex.lock();
    }
}

} // namespace condition_variable
