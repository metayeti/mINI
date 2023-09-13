#!/bin/bash
cd $(dirname $(realpath "$0"))
for file in `find . -type f -name "test*.cpp"`
do
   bn=$(basename -s .cpp $file)
   echo "Building $bn..."
   ./build.sh $bn
   echo "Run $bn..."
   ./run.sh $bn
   if [ $? -ne 0 ]
   then
        echo -e "\033[41mTEST $bn FAILED\033[0m"
        echo "Would you like to continue testing? (y/n) "
        read choice
        if [ $choice = n ]
        then
            echo "Testing was stopped by the user"
            break
        fi
   fi
done
./clean.sh
