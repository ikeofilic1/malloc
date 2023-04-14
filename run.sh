#!/usr/bin/env bash

make
for lib in lib/*; do
    echo -e "\x1b[33;1m\nTesting $lib...\x1b[0m"
    for test in $(find tests -type f -executable); do
        if env LD_PRELOAD=$lib $test; then
            echo -e "\n\t\x1b[32;1m$test passed\n"
        else
            echo -e "\n\t\x1b[31;1m$test failed\n"
        fi
        echo -e -n "\x1b[0m"
    done
done
