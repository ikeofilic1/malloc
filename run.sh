#!/usr/bin/env bash

# make
# for lib in lib/*; do
#     echo -e "\x1b[33;1m\nTesting $lib...\x1b[0m"
#     for test in $(find tests -type f -executable); do
#         if env LD_PRELOAD="$lib" "$test"; then
#             echo -e "\n\t\x1b[32;1m$test passed\n"
#         else
#             echo -e "\n\t\x1b[31;1m$test failed\n"
#         fi
#         echo -e -n "\x1b[0m"
#     done
# done

make perf

CSV=times.csv
echo Test name, First fit, Next fit, Best fit, Worst fit, Malloc > $CSV

echo -e "\x1b[20;1mNow running performance tests\x1b[0m\n"
for test in $(find performance-tests -type f -executable); do

    file=${test}-stats; : > "$file"

    temp="$(basename "$test"), "
    echo -e "\x1b[33;1mRunning $(basename "$test")...\x1b[0m\n"

    for lib in lib/libmalloc-ff.so lib/libmalloc-nf.so \
        lib/libmalloc-bf.so lib/libmalloc-wf.so; do
        
        echo "=========$lib==========" >> "$file"
        temp+=$({ command time -f "%e, " env LD_PRELOAD=$lib "$test" 1>> "$file"; } 2>&1)
        echo -e "======================================\n" >> "$file"

    done

    temp+=$({ command time -f "%e" "$test" 1>> "$file"; } 2>&1)

    echo "$temp" >> $CSV
done

