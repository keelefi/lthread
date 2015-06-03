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
};

} // namespace lthread
