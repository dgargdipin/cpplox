add_library(lox)
target_include_directories(lox
        PUBLIC
        ${lox_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_sources(
        lox
        PUBLIC
        lox.cpp
        RuntimeException.cpp
        scanner.cpp
        utils.cpp
        Environment.cpp
        parser.cpp
        interpreter.cpp
        Clock.cpp
        LoxFunction.cpp
)
add_executable(lox_repl)
set_target_properties(lox_repl PROPERTIES OUTPUT_NAME "lox")

target_include_directories(lox_repl
        PUBLIC
        ${lox_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_sources(lox_repl
        PRIVATE
        main.cpp
        )

target_link_libraries(lox_repl PRIVATE lox)
add_executable(generate_ast generateast.cpp)
