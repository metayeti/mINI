#!/bin/bash
cd build/

rm -f *.ini

for test_file in *.test; do
    ./$test_file -p > /dev/null
    if [[ $? -ne 0 ]]; then
        echo "Test $test_file failed."
        exit 1
    fi
    echo "Test $test_file passed."
done
