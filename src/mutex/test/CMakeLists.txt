add_executable(OneMutexNonTimedTests
    one_mutex_non-timed_tests.cc
    )

target_link_libraries(OneMutexNonTimedTests
    ${GTEST_BOTH_LIBRARIES}
    mutex
    futex
    gettid
    exception
    pthread
    )

add_executable(OneMutexTimedTests
    one_mutex_timed_tests.cc
    )

target_link_libraries(OneMutexTimedTests
    ${GTEST_BOTH_LIBRARIES}
    mutex
    futex
    gettid
    exception
    pthread
    )

add_executable(DeadlockTests
    deadlock_tests.cc
    )

target_link_libraries(DeadlockTests
    ${GTEST_BOTH_LIBRARIES}
    mutex
    futex
    gettid
    exception
    pthread
    )

LTHREAD_ADD_TEST(OneMutexNonTimedTests)
LTHREAD_ADD_TEST(OneMutexTimedTests)
LTHREAD_ADD_TEST(DeadlockTests)
