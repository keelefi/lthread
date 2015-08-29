add_executable(ConditionVariableTests
    condition_variable_tests.cc
    )

target_link_libraries(ConditionVariableTests
    ${GTEST_BOTH_LIBRARIES}
    condition_variable
    mutex
    futex
    gettid
    exception
    pthread
    )

LTHREAD_ADD_TEST(ConditionVariableTests)
