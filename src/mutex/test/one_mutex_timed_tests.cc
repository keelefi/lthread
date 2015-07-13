#include "mutex.hh"

#include "gtest/gtest.h"

#include <atomic>
#include <thread>

class OneMutexTimedTests: public testing::Test
{
public:
    lthread::mutex m_mutex;

    OneMutexTimedTests() :
            m_mutex()
    {
    }
};

/**
 * Use two threads that try to acquire the same mutex and see if the second
 * one blocks.
 *
 * Timeline (in seconds):
 *    0: start thread 1 that grabs the lock and sleeps for 3 seconds
 *    1: start thread 2 that tries to grab the lock and set the bool variable
 *    2: expect that thread 2 didn't get the lock and the bool variable is
 *       unset
 *    3: thread 1 releases the lock and ends
 *       thread 2 gets the lock, sets the flag and ends
 *       test case ends
 *
 * Note: Regardless if the mutual exclusion works, this test should take
 * 3 seconds (and a little more) to execute.
 */
TEST_F(OneMutexTimedTests, two_threads_access_same_mutex_second_blocks)
{
    std::atomic<bool> thread2_got_lock(false);

    std::thread thread1(
            [this]
            {
                m_mutex.lock();
                std::this_thread::sleep_for(std::chrono::seconds(3));
                m_mutex.unlock();
            });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thread2(
            [this, &thread2_got_lock]
            {
                m_mutex.lock();
                thread2_got_lock.store(true, std::memory_order_relaxed);
                m_mutex.unlock();
            });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_FALSE(thread2_got_lock.load(std::memory_order_relaxed));

    thread1.join();
    thread2.join();
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
