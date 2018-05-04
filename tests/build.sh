#!/bin/bash
if [[ $# -eq 0 ]] ; then
    echo Use: $0 [test name]
	exit 1
fi
g++ -Wall -std=c++11 -I./lest -I./../src -o $1.test $1.cpp ./../src/mini/ini.cpp
