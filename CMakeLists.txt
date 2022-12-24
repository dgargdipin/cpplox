cmake_minimum_required(VERSION 3.22)
project(lox)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(lox
        lox.cpp
        lox.h
        main.cpp
        scanner.cpp
        scanner.h
        utils.cpp
        utils.h
        Expr.hpp
        parser_def.h
        astprinter.h RuntimeException.cpp RuntimeException.h)
