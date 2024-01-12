cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(cpputilssrc VERSION 0.1 LANGUAGES CXX)

add_library(cpputilssrc SHARED
    include/cpputils/export.hpp
    include/cpputils/result.hpp
    include/cpputils/event.hpp
    include/cpputils/ai/behaviourtree.hpp

    result.cpp
    event.cpp
    ai/behaviourtree.cpp
)

target_include_directories(cpputilssrc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)