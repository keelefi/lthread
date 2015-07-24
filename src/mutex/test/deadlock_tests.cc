#include "mutex.hh"

#include "exception.hh"

#include "gtest/gtest.h"

#include <future>
#include <thread>

class DeadlockTests: public testing::Test
{
public:
    lthread::mutex m_mutexA;
    lthread::mutex m_mutexB;

    DeadlockTests() :
            m_mutexA(true),
            m_mutexB(true)
    {
    }
};

/**
 * Lock the same mutex twice without unlocking in between. The first lock() must
 * not throw an exception, but the second lock() should throw an exception
 * indicating a deadlock.
 */
TEST_F(DeadlockTests, AA_deadlock_throws_exception)
{
    try
    {
        m_mutexA.lock();
    }
    catch (lthread::exception& e)
    {
        // fail test if we reach here
        ASSERT_TRUE(false);
    }

    try
    {
        m_mutexA.lock();

        // fail test if we reach here
        ASSERT_TRUE(false);
    }
    catch (lthread::exception &e)
    {
        EXPECT_STREQ(
                "futex_wait() failed with: 35 - Resource deadlock avoided",
                e.what());
    }

    // clean-up
    m_mutexA.unlock();
}

/**
 * Lock the mutexes A and B from two different threads, in different order.
 * First thread1 locks A, then thread2 locks B. Now, thread1 tries to lock B
 * after which upon thread2 trying to lock A will receive an exception
 * indicating a deadlock.
 *
 *      thread1                 thread2
 *      -------                 -------
 *      lock A
 *                              lock B
 *      lock B (wait here)
 *                              lock A (deadlock)   
 *
 * Note: This implementation uses sleep_for(). It would be nice to somehow get
 * rid of that.
 */
TEST_F(DeadlockTests, ABBA_deadlock_throws_exception)
{
    std::promise<bool> promise_from_1;
    std::promise<bool> promise_from_2;

    std::future<bool> future_for_2 = promise_from_1.get_future();
    std::future<bool> future_for_1 = promise_from_2.get_future();

    std::thread thread2(
            [this, &promise_from_2, &future_for_2]
            {
                future_for_2.wait();

                try
                {
                    m_mutexB.lock();
                }
                catch (lthread::exception& e)
                {
                    // fail test if we reach here
                    ASSERT_TRUE(false);
                }

                std::cout << "thread2 locked B" << std::endl;

                promise_from_2.set_value(true);

                // Wait for thread1 to start waiting for mutex B.
                std::this_thread::sleep_for(std::chrono::seconds(1));

                std::cout << "thread2 trying to lock A" << std::endl;

                try
                {
                    m_mutexA.lock();

                    // fail test if we reach here
                    ASSERT_TRUE(false);
                }
                catch (lthread::exception& e)
                {
                    EXPECT_STREQ(
                            "futex_wait() failed with: 35 - Resource deadlock "
                                    "avoided",
                            e.what());

                    m_mutexB.unlock();

                    std::cout << "thread2 unlocked B" << std::endl;
                }
            });

    try
    {
        m_mutexA.lock();
    }
    catch (lthread::exception& e)
    {
        // fail test if we reach here
        ASSERT_TRUE(false);
    }

    std::cout << "thread1 locked A" << std::endl;

    promise_from_1.set_value(true);
    future_for_1.wait();

    std::cout << "thread1 trying to lock B" << std::endl;

    try
    {
        m_mutexB.lock();
    }
    catch (lthread::exception &e)
    {
        // Even though we deadlocked, we shouldn't reach here since thread2
        // catched the exception and unlocks B. Therefore, if we reach here we
        // fail the test.
        ASSERT_TRUE(false);
    }

    // clean-up
    thread2.join();
    m_mutexB.unlock();
    m_mutexA.unlock();
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
