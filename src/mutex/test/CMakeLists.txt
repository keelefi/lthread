add_executable(OneMutexTimedTests one_mutex_timed_tests.cc)
target_link_libraries(OneMutexTimedTests ${GTEST_BOTH_LIBRARIES} pthread)
GTEST_ADD_TESTS(OneMutexTimedTests "" one_mutex_timed_tests.cc)
