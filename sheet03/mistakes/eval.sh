#!/bin/bash
set -euo pipefail
rm -rf build

cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER="clang++-18" -DCMAKE_C_COMPILER="clang-18" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --target tests

FAILED=0
clang-format  --dry-run --Werror fixups.hpp  || { FAILED=1; echo 'FAILED: code formatting. Run `clang-format -i *.cpp`.'; }
# checks whether no "real" code was added only preprocessor statements
clang++-18 -E fixups.hpp | [ `wc -l` = 8 ] || { FAILED=1; echo 'FAILED: nothing allowed in `fixups.hpp` apart from preprocessor directives'; }
build/tests || { FAILED=1; echo 'FAILED: Tests failed'; }
[[ $FAILED == 0 ]] && echo "Tests PASSED." || echo "Some tests or checks FAILED."
exit $FAILED