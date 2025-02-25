cmake_minimum_required(VERSION 3.5)
project(mINI_test CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXECUTABLE_SUFFIX ".test")

# Check GCC version and add -lstdc++fs if necessary
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_LESS 9.1)
        link_libraries("-lstdc++fs")
    endif()
endif()

# Collect source files
file(GLOB SRC_FILES "test*.cpp")

include_directories("lest" "../src")
    
# Filter to build each test exetuable
foreach(_source IN ITEMS ${SRC_FILES})
    get_filename_component(_source_name "${_source}" NAME_WE)
    add_executable(${_source_name} ${_source})
endforeach()
