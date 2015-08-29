#include "condition_variable.hh"
#include "mutex.hh"

#include "gtest/gtest.h"

#include "exception.hh"

#include <future>
#include <thread>

class ConditionVariableTests: public testing::Test
{
public:
    lthread::mutex m_mutex;
    lthread::condition_variable m_condition_variable;

    ConditionVariableTests() :
            m_condition_variable()
    {
    }
};

TEST_F(ConditionVariableTests, wait_for_condition_variable_from_other_thread)
{
    bool flag = false;
    std::promise<bool> promise_from_main;

    std::thread signalling_thread(
            [this, &flag] (std::future<bool> future)
            {
                future.get();

                m_mutex.lock();

                flag = true;

                m_condition_variable.signal(m_mutex);
            },
            promise_from_main.get_future());

    m_mutex.lock();

    promise_from_main.set_value(true);

    while (!flag)
    {
        m_condition_variable.wait(m_mutex);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
