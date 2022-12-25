add_executable(unit_test ScannerTests..cpp)
add_dependencies(check unit_test)
target_sources(unit_test
        PRIVATE
        main.cpp
        ScannerTests..cpp
        )
set(EXECUTABLE_NAME "unit_test")
set_target_properties(unit_test PROPERTIES
        OUTPUT_NAME ${EXECUTABLE_NAME}
        )
target_link_libraries(unit_test
        PUBLIC
        gtest
        lox
        )
include(GoogleTest)
gtest_discover_tests(unit_test)
