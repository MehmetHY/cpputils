cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(cpputilssrc VERSION 0.1 LANGUAGES CXX)

add_library(cpputilssrc INTERFACE
    include/cpputils/any.hpp
    include/cpputils/event.hpp
    include/cpputils/export.hpp
    include/cpputils/nullable.hpp
    include/cpputils/result.hpp

    include/cpputils/ai/behaviourtree.hpp
    include/cpputils/ai/statemachine.hpp
)

add_library(cpputils::cpputils ALIAS cpputilssrc)

target_include_directories(cpputilssrc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)