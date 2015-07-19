#pragma once

namespace lthread
{

class mutex
{
public:
    mutex();
    ~mutex();

    void lock();
    void unlock();

private:
    int m_val;
};

} // namespace lthread
