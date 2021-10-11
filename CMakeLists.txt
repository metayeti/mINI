cmake_minimum_required(VERSION 3.0)

project(mINI LANGUAGES CXX)

add_library(mINI INTERFACE)
add_library(mINI::mINI ALIAS mINI)

target_include_directories(mINI SYSTEM INTERFACE src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
