#include "mutex.hh"

#include "exception.hh"

#include "gtest/gtest.h"

#include <thread>

class DeadlockTests: public testing::Test
{
public:
    lthread::mutex m_mutexA;

    DeadlockTests() :
            m_mutexA()
    {
    }
};

/**
 * Lock the same mutex twice without unlocking in between. The first lock() must
 * not throw an exception, but the second lock() should throw an exception
 * indicating a deadlock.
 *
 * Note: this test is currently broken, because Linux checks for deadlocks only
 * when priority inheritance mutexes are used. Our futex implementation doesn't
 * yet support PI mutexes.
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
                "futex_wait() failed with 35 - Resource deadlock avoided",
                e.what());
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
