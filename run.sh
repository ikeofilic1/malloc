#!/usr/bin/env bash

make
for lib in lib/*; do
    echo "Testing $lib..."
    for test in $(find tests -type f -executable); do
        if env LD_PRELOAD=$lib $test > /dev/null; then
            echo -e "\t\x1b[32;1m$test passed"
        else
            echo -e "\t\x1b[31;1m$test failed"
        fi
        echo -e -n "\x1b[0m"
    done
done
