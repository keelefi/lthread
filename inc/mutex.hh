#pragma once

namespace lthread
{

class mutex
{
public:
    mutex();
    mutex(bool pi);
    ~mutex();

    void lock();
    void unlock();

private:
    void lock_pi();
    void unlock_pi();

    void lock_nonpi();
    void unlock_nonpi();

    int m_val;
    bool m_pi;
};

} // namespace lthread
