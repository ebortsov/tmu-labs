#!/bin/bash
set -euo pipefail
rm -rf build

cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER="clang++-18" -DCMAKE_C_COMPILER="clang-18" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --target tests

FAILED=0
clang-format  --dry-run --Werror tests/*.cpp  || { FAILED=1; echo 'FAILED: code formatting. Run `clang-format -i *.cpp`.'; }
LLVM_PROFILE_FILE=build/testcoverage.profraw build/tests || { FAILED=1; echo "FAILED: tests failed"; };
if [[ $FAILED != 0 ]]; then echo "Some tests or checks FAILED. Exiting early."; exit $FAILED; fi

llvm-profdata merge -o build/testcoverage.profdata build/testcoverage.profraw || { FAILED=1; echo "FAILED: no coverage report generated"; exit $FAILED; };
if [[ $(llvm-cov show build/tests --instr-profile=build/testcoverage.profdata --line-coverage-lt=99) ]]; 
then
    FAILED=1; 
    echo "You didn't reach the coverage goal yet! Missing coverage:";
    llvm-cov show build/tests --instr-profile=build/testcoverage.profdata --line-coverage-lt=99
    exit $FAILED;
fi

clang-tidy -p build/compile-commands.json --config-file="../.clang-tidy" tests/*.cpp || { FAILED=1; echo 'FAILED: clang-tidy failed'; }
[[ $FAILED == 0 ]] && echo "Tests PASSED." || echo "Some tests or checks FAILED."
exit $FAILED