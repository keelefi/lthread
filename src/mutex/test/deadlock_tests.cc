#include "mutex.hh"

#include "exception.hh"

#include "gtest/gtest.h"

#include <atomic>
#include <future>
#include <thread>

class DeadlockTests: public testing::Test
{
public:
    lthread::mutex m_mutex1;
    lthread::mutex m_mutex2;
    lthread::mutex m_mutex3;

    DeadlockTests() :
            m_mutex1(true),
            m_mutex2(true),
            m_mutex3(true)
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
        m_mutex1.lock();
    }
    catch (lthread::exception& e)
    {
        // fail test if we reach here
        ASSERT_TRUE(false);
    }

    try
    {
        m_mutex1.lock();

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
    m_mutex1.unlock();
}

/**
 * Lock the mutexes 1 and 2 from two different threads, in different order.
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
 *      threadA                 threadB
 *      -------                 -------
 *      lock 1
 *                              lock 2
 *      lock 2 (wait here)
 *                              lock 1 (deadlock)   
 */
TEST_F(DeadlockTests, ABBA_deadlock_throws_exception)
{
    std::promise<bool> promise_from_A;
    std::promise<bool> promise_from_B;

    std::promise<bool> promise_for_A;
    std::promise<bool> promise_for_B;

    std::atomic<bool> fail(false);

    std::thread threadA(
            lock_mutex_and_signal_and_wait_and_try_locking_second,
            std::ref(m_mutex1),
            std::ref(m_mutex2),
            std::ref(promise_from_A),
            promise_for_A.get_future(),
            std::ref(fail));
    std::thread threadB(
            lock_mutex_and_signal_and_wait_and_try_locking_second,
            std::ref(m_mutex2),
            std::ref(m_mutex1),
            std::ref(promise_from_B),
            promise_for_B.get_future(),
            std::ref(fail));

    promise_from_A.get_future().wait();
    promise_from_B.get_future().wait();

    promise_for_A.set_value(true);
    promise_for_B.set_value(true);

    threadA.join();
    threadB.join();

    EXPECT_EQ(fail.load(std::memory_order_seq_cst), true);
}

/**
 * Like the ABBA deadlock test, but with 3 threads and 3 mutexes.
 */
TEST_F(DeadlockTests, three_threads_deadlock_throws_exception)
{
    std::promise<bool> promise_from_A;
    std::promise<bool> promise_from_B;
    std::promise<bool> promise_from_C;

    std::promise<bool> promise_for_A;
    std::promise<bool> promise_for_B;
    std::promise<bool> promise_for_C;

    std::atomic<bool> fail(false);

    std::thread threadA(
            lock_mutex_and_signal_and_wait_and_try_locking_second,
            std::ref(m_mutex1),
            std::ref(m_mutex2),
            std::ref(promise_from_A),
            promise_for_A.get_future(),
            std::ref(fail));
    std::thread threadB(
            lock_mutex_and_signal_and_wait_and_try_locking_second,
            std::ref(m_mutex2),
            std::ref(m_mutex3),
            std::ref(promise_from_B),
            promise_for_B.get_future(),
            std::ref(fail));
    std::thread threadC(
            lock_mutex_and_signal_and_wait_and_try_locking_second,
            std::ref(m_mutex3),
            std::ref(m_mutex1),
            std::ref(promise_from_C),
            promise_for_C.get_future(),
            std::ref(fail));

    promise_from_A.get_future().wait();
    promise_from_B.get_future().wait();
    promise_from_C.get_future().wait();

    promise_for_A.set_value(true);
    promise_for_B.set_value(true);
    promise_for_C.set_value(true);

    threadA.join();
    threadB.join();
    threadC.join();

    EXPECT_EQ(fail.load(std::memory_order_seq_cst), true);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
