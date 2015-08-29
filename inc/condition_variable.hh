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
    lthread::mutex m_mutex;
};

} // namespace lthread
