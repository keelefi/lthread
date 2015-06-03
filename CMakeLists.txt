cmake_minimum_required(VERSION 3.0.2)

project(lthread)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(src)
