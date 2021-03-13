cmake_minimum_required(VERSION 3.8)
project(mini)
set(CMAKE_CXX_STANDARD 14)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/tests/lest)

add_executable(testcasesens ${PROJECT_SOURCE_DIR}/tests/testcasesens.cpp)
add_executable(testcopy ${PROJECT_SOURCE_DIR}/tests/testcopy.cpp)
add_executable(testgenerate ${PROJECT_SOURCE_DIR}/tests/testgenerate.cpp)
add_executable(testhuge ${PROJECT_SOURCE_DIR}/tests/testhuge.cpp)
add_executable(testread ${PROJECT_SOURCE_DIR}/tests/testread.cpp)
add_executable(testutf8 ${PROJECT_SOURCE_DIR}/tests/testutf8.cpp)
add_executable(testwrite ${PROJECT_SOURCE_DIR}/tests/testwrite.cpp)

add_test(testcasesens testcasesens)
add_test(testcopy testcopy) 
add_test(testgenerate testgenerate)
add_test(testhuge testhuge)
add_test(testread testread)
add_test(testutf8 testutf8)
add_test(testwrite testwrite)