#!/usr/bin/env zsh

# Run this file, it will give all output
tests=("2 + 3 / 23 * (|23 * |3 - -4|| + 4)"
       "a = 2\na + 1 / a"
       "sin(2 + 3) * |-4|"
       "fun d(x) = x * 2\na = 4\nd(a)"
       "fun abs2(x) = if x < 0 { -x } else { x }\nabs2(-2)\nabs2(8)")
for t in $tests; do
    echo "test with \n$t"
    echo "------------"
    echo $t | ./cmake-build-debug/lang
    echo "--- done ---"
    echo
done
