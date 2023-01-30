cmake_minimum_required(VERSION 3.24)
project(lox)

set(CMAKE_CXX_STANDARD 17)
include(CTest)

add_subdirectory(dependencies)
add_subdirectory(src)
#include_directories(include)
if(BUILD_TESTING AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)
    add_subdirectory(test)
endif()

#add_executable(lox
#        include/Expr.hpp
#        include/interpreter.h
#        include/logger.h
#        include/lox.h
#        include/parser_def.h
#        include/RuntimeException.h
#        include/scanner.h
#        include/utils.h
#        src/lox.cpp
#        src/main.cpp
#        src/RuntimeException.cpp
#        src/scanner.cpp
#        src/utils.cpp)
