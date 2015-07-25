#include "mutex.hh"
#include "exception.hh"

#include "gtest/gtest.h"

#include <thread>
#include <vector>

class OneMutexNonTimedTests: public testing::Test
{
public:
    lthread::mutex m_mutex;
    lthread::mutex m_mutex_pi;

    OneMutexNonTimedTests() :
            m_mutex(false),
            m_mutex_pi(true)
    {
    }
};

/**
 * 100 threads try to increment the same variable 10000 times. The variable is
 * expected to hold 100 * 10000 at the end of the test.
 */
TEST_F(OneMutexNonTimedTests, one_hundred_threads_update_same_variable)
{
    unsigned int thread_count = 100;
    unsigned int increments_per_thread = 10000;
    std::vector<std::thread> threads;
    int variable = 0;

    for (unsigned int i = 0; i < thread_count; ++i)
    {
        threads.emplace_back(
                [this, &increments_per_thread, &variable]
                {
                    for (unsigned int i = 0; i < increments_per_thread; ++i)
                    {
                        try
                        {
                            m_mutex.lock();
                        }
                        catch (lthread::exception& e)
                        {
                            std::cout << e.what();
                            ASSERT_TRUE(false);
                        }

                        ++variable;

                        try
                        {
                            m_mutex.unlock();
                        }
                        catch (lthread::exception& e)
                        {
                            std::cout << e.what();
                            ASSERT_TRUE(false);
                        }
                    }
                });
    }

    for (unsigned int i = 0; i < thread_count; ++i)
    {
        threads[i].join();
    }

    EXPECT_EQ(thread_count * increments_per_thread, variable);
}
TEST_F(OneMutexNonTimedTests, one_hundred_threads_update_same_variable_pi)
{
    unsigned int thread_count = 40;
    unsigned int increments_per_thread = 100;
    std::vector<std::thread> threads;
    int variable = 0;

    for (unsigned int i = 0; i < thread_count; ++i)
    {
        threads.emplace_back(
                [this, &increments_per_thread, &variable]
                {
                    for (unsigned int i = 0; i < increments_per_thread; ++i)
                    {
                        try
                        {
                            m_mutex_pi.lock();
                        }
                        catch (lthread::exception& e)
                        {
                            std::cout << e.what();
                            ASSERT_TRUE(false);
                        }

                        ++variable;

                        try
                        {
                            m_mutex_pi.unlock();
                        }
                        catch (lthread::exception& e)
                        {
                            std::cout << e.what();
                            ASSERT_TRUE(false);
                        }
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
