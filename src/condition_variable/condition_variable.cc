#include "condition_variable.hh"

namespace lthread
{

condition_variable::condition_variable() :
        m_mutex(true)
{
}

void condition_variable::signal(lthread::mutex& mutex)
{
}

void condition_variable::wait(lthread::mutex& mutex)
{
}

} // namespace condition_variable
