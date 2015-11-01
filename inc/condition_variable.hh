#pragma once

#include "mutex.hh"

namespace lthread
{

class condition_variable
{
public:
    condition_variable();

    void signal(lthread::mutex& mutex);

    void wait(lthread::mutex& mutex);

private:
    int m_seq_no_waiter;
    int m_seq_no_wakeup;
};

} // namespace lthread
