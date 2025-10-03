cmake_minimum_required(VERSION 3.5)
project(mINI CXX)
set(CMAKE_CXX_STANDARD 17)

# Check if we need to link filesystem library
# For GCC < 9 and some other compilers, std::filesystem requires linking
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    string(STRIP "${GCC_VERSION}" GCC_VERSION)
    if (GCC_VERSION VERSION_LESS 9.1)
        set(FILESYSTEM_LIB "stdc++fs")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Some versions of Clang also need explicit filesystem linking
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        set(FILESYSTEM_LIB "c++fs")
    endif()
endif()

add_library(mINI INTERFACE)
target_include_directories(mINI INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Link filesystem library if needed
if (FILESYSTEM_LIB)
    target_link_libraries(mINI INTERFACE ${FILESYSTEM_LIB})
endif()
