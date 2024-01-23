cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(cpputils VERSION 0.1 LANGUAGES CXX)

option(BUILD_TESTING_CPPUTILS "Build cpputils tests" OFF)

add_subdirectory(cpputils)

if(BUILD_TESTING_CPPUTILS)
    add_subdirectory(tests)
endif(BUILD_TESTING_CPPUTILS)
