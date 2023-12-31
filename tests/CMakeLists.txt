cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(cpputilstests VERSION 0.1 LANGUAGES CXX)

enable_testing()

add_executable(cpputilstests
    result.cc
)

include(FetchContent)

# googletest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(cpputilstests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(cpputilstests)

target_link_libraries(cpputilstests PRIVATE cpputilssrc)