add_test([=[ScannerTests.Basic]=]  /home/dgargdipin/dev/lox/cmake-build-debug-wsl/test/unit_test [==[--gtest_filter=ScannerTests.Basic]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ScannerTests.Basic]=]  PROPERTIES WORKING_DIRECTORY /home/dgargdipin/dev/lox/cmake-build-debug-wsl/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ScannerTests.Comment]=]  /home/dgargdipin/dev/lox/cmake-build-debug-wsl/test/unit_test [==[--gtest_filter=ScannerTests.Comment]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ScannerTests.Comment]=]  PROPERTIES WORKING_DIRECTORY /home/dgargdipin/dev/lox/cmake-build-debug-wsl/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ScannerTests.Expression]=]  /home/dgargdipin/dev/lox/cmake-build-debug-wsl/test/unit_test [==[--gtest_filter=ScannerTests.Expression]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ScannerTests.Expression]=]  PROPERTIES WORKING_DIRECTORY /home/dgargdipin/dev/lox/cmake-build-debug-wsl/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  unit_test_TESTS ScannerTests.Basic ScannerTests.Comment ScannerTests.Expression)