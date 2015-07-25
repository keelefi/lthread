cmake_minimum_required(VERSION 3.0.2)

project(lthread)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

function(LTHREAD_ADD_TEST test)
    add_test(${test} ${test})
    add_test(${test}-valgrind valgrind ./${test})
endfunction(LTHREAD_ADD_TEST)

add_subdirectory(src)
