#pragma once

#include <fstream>
#include <string>
#include "LoxExceptions.h"
#include "token.h"

std::string readTextFile(const std::string &path);


namespace Lox {
//    extern bool had_error;

    void run(std::string input,bool print_expressions);

    void report(int line, const std::string &where, const std::string &message);

    void runFile(std::string path);

    void runPrompt();

    void error(int line, std::string message);

    void error(Token token, std::string message);

    void runtime_error(RuntimeException& e);

};