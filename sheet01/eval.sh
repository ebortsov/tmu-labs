#!/bin/bash
set -euo pipefail
rm -rf build

# Verify that code adheres the formatting style
clang-format --dry-run --Werror *.cpp || { FAILED=1; echo 'FAILED: code formatting. Run `clang-format -i *.cpp`.'; }

# Configure and compile
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER="clang++-18" -DCMAKE_C_COMPILER="clang-18" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Werror"
cmake --build build --target helloworld

FAILED=0
testcase() { build/helloworld "${@:2}" | cmp -s <(echo -ne "$1") || { FAILED=1; echo "FAILED: '$1'"; }; }

# Functionality test cases
# Format is: testcase "expected output" program-arguments...
testcase "Hello World!\n"
testcase "Hello abc!\n" "abc"
testcase "Hello c++ homework!\n" "c++ homework"

# clang-tidy is slow, so only run if all prior tests passed
if [[ $FAILED != 0 ]]; then echo "Some tests or checks FAILED. Exiting early."; exit $FAILED; fi
clang-tidy -p build/compile-commands.json --config-file=".clang-tidy" *.cpp || { FAILED=1; echo 'FAILED: clang-tidy failed'; }

[[ $FAILED == 0 ]] && echo "Tests PASSED." || echo "Some tests or checks FAILED."
exit $FAILED
