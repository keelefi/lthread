#include "mutex.hh"

#include "exception.hh"

#include "gtest/gtest.h"

#include <atomic>
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

static void lock_mutex_and_signal_and_wait_and_try_locking_second(
        lthread::mutex& mutex_first,
        lthread::mutex& mutex_second,
        std::promise<bool>& promise,
        std::future<bool> future,
        std::atomic<bool>& fail)
{
    try
    {
        mutex_first.lock();
    }
    catch (lthread::exception& e)
    {
        // fail test if we reach here
        ASSERT_TRUE(false);
    }

    promise.set_value(true);
    future.wait();
    // TODO: Does set_value() imply a release memory barrier and wait() imply
    // an acquire memory barrier or could the locking of the first/second mutex
    // jump downwards/upwards, respectively?

    bool second_lock_succeeded = false;
    try
    {
        mutex_second.lock();

        second_lock_succeeded = true;
    }
    catch (lthread::exception& e)
    {
        if (fail.exchange(true, std::memory_order_seq_cst))
        {
            // Deadlock was detected already previously. Fail the test.
            ASSERT_TRUE(false);
        }
        else
        {
            EXPECT_STREQ(
                    "futex_wait() failed with: 35 - Resource deadlock avoided",
                    e.what());
        }
    }

    if (second_lock_succeeded)
    {
        mutex_second.unlock();
    }

    mutex_first.unlock();
}

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
 * Both threads lock their first mutexes and then signal the main thread. Once
 * the main thread sees the indication from each one of the threads, it signals
 * back that the threads are free to lock the second mutexes. The first thread
 * to react will block on trying to lock the second mutex. The second thread to
 * react will fail the mutex lock() operation with an exception indicating
 * deadlock avoidance.
 *
 * Which thread will fail the deadlock is random. One possible scenario is the
 * following:
 *
 *      thread1                 thread2
 *      -------                 -------
 *      lock A
 *                              lock B
 *      lock B (wait here)
 *                              lock A (deadlock)   
 */
TEST_F(DeadlockTests, ABBA_deadlock_throws_exception)
{
    std::promise<bool> promise_from_1;
    std::promise<bool> promise_from_2;

    std::promise<bool> promise_for_1;
    std::promise<bool> promise_for_2;

    std::atomic<bool> fail(false);

    std::thread thread1(
            lock_mutex_and_signal_and_wait_and_try_locking_second,
            std::ref(m_mutexA),
            std::ref(m_mutexB),
            std::ref(promise_from_1),
            promise_for_1.get_future(),
            std::ref(fail));
    std::thread thread2(
            lock_mutex_and_signal_and_wait_and_try_locking_second,
            std::ref(m_mutexB),
            std::ref(m_mutexA),
            std::ref(promise_from_2),
            promise_for_2.get_future(),
            std::ref(fail));

    promise_from_1.get_future().wait();
    promise_from_2.get_future().wait();

    promise_for_1.set_value(true);
    promise_for_2.set_value(true);

    thread1.join();
    thread2.join();

    EXPECT_EQ(fail.load(std::memory_order_seq_cst), true);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
