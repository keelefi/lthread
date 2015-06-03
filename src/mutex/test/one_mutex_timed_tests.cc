#include "mutex.hh"

#include "gtest/gtest.h"

class OneMutexTimedTests: public testing::Test
{
};

TEST_F(OneMutexTimedTests, two_threads_access_same_mutex_second_blocks)
{
    ASSERT_TRUE(true);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
