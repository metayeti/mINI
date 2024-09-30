cmake_minimum_required(VERSION 3.5)
project(mINI CXX)
set(CMAKE_CXX_STANDARD 11)

add_library(mINI INTERFACE)
target_include_directories(mINI INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")
