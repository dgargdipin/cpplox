#pragma once

#include <fstream>
#include <string>
#include "scanner.h"

std::string readTextFile(const std::string &path);


namespace Lox {
    extern bool had_error;

    void run(std::string input);

    void report(int line, const std::string &where, const std::string &message);

    void runFile(std::string path);

    void runPrompt();

    void error(int line, std::string message);

    void error(Token token, std::string message);

};