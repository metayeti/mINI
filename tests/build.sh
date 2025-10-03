#!/bin/bash
if [[ $# -eq 0 ]] ; then
    echo Use: $0 [test name]
	exit 1
fi

# Check if we need to link filesystem library (for GCC < 9 and some systems)
FILESYSTEM_LIB=""
if command -v g++ >/dev/null 2>&1; then
    GCC_VERSION=$(g++ -dumpversion)
    if [[ "$GCC_VERSION" < "9" ]]; then
        FILESYSTEM_LIB="-lstdc++fs"
    fi
fi

# Try to compile with filesystem library first, fallback without if it fails
g++ -Wall -Wextra -std=c++17 -I./lest -I./../src $FILESYSTEM_LIB -o $1.test $1.cpp 2>/dev/null || \
g++ -Wall -Wextra -std=c++17 -I./lest -I./../src -o $1.test $1.cpp
