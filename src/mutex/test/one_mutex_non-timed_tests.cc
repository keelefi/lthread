#include "mutex.hh"

#include "gtest/gtest.h"

#include <thread>
#include <vector>

class OneMutexNonTimedTests: public testing::Test
{
public:
    lthread::mutex m_mutex;

    OneMutexNonTimedTests() :
            m_mutex()
    {
    }
};

/**
 * 100 threads try to increment the same variable 10000 times. The variable is
 * expected to hold 100 * 10000 at the end of the test.
 */
TEST_F(OneMutexNonTimedTests, one_hundred_threads_update_same_variable)
{
    int thread_count = 100;
    int increments_per_thread = 10000;
    std::vector<std::thread> threads;
    int variable = 0;

    for (unsigned int i = 0; i < thread_count; ++i)
    {
        threads.emplace_back(
                [this, &increments_per_thread, &variable]
                {
                    for (unsigned int i = 0; i < increments_per_thread; ++i)
                    {
                        m_mutex.lock();
                        ++variable;
                        m_mutex.unlock();
                    }
                });
    }

    for (unsigned int i = 0; i < thread_count; ++i)
    {
        threads[i].join();
    }

    EXPECT_EQ(thread_count * increments_per_thread, variable);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
