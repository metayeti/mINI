@echo off
IF %1.==. GOTO ERR
g++ -Wall -std=c++11 -I./lest -I./../src -o %1.exe %1.cpp ./../src/mini/ini.cpp
GOTO END
:ERR
echo Use: %0 [test name]
:END