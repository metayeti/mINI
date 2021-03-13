cmake_minimum_required(VERSION 3.8)
project(mini)
set(CMAKE_CXX_STANDARD 14)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/tests/lest)

macro(add_test_macro test_name)
    add_executable(${test_name} ${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_name}.cpp)
    add_test(${test_name} ${test_name})
endmacro()

add_test_macro(testcasesens)
add_test_macro(testcopy)
add_test_macro(testgenerate)
add_test_macro(testhuge)
add_test_macro(testread)
add_test_macro(testutf8)
add_test_macro(testwrite)