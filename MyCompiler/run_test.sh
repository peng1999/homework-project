#!/usr/bin/env zsh

# Run this file, it will give all output
tests=("2 + 3 / 23 * (|23 * |3 - -4|| + 4)"
       "a = 1\na + 1"
       "sin(2 + 3) * |-4|")
for t in $tests; do
    echo "test with $t:"
    echo $t | ./cmake-build-debug/lang
    echo "--- done ---"
done
