#include <iostream>
#include <utility>
#include "lox.h"
#include "utils.h"
#include "parser_def.h"
#include <sysexits.h>
#include "astprinter.h"

std::string readTextFile(const std::string &path) {
    std::string res;
    std::ifstream file(path);
    if (file.fail()) {
        error("Cannot open input file", path);
    }
    std::string curr;
    while (file) {
        file >> curr;
        res += curr;
    }
    if (file.fail()) {
        std::cout << "WARNING: file input error" << std::endl;
    }
    return res;
}

namespace Lox {
    bool had_error = false;
}

void Lox::run(std::string input) {
    Scanner scanner(std::move(input));
    auto tokens = scanner.scanTokens();

    ParserClass<std::string> parser(tokens);
//    p.print();
//    print();
    auto expression = parser.parseTokens();
    if (had_error)return;
    std::cout << ASTPrinter().print(expression);
//
//    std::cout<<tokens.size()<<std::endl;

    // for (auto token : tokens)
    // {
    //     std::cout << token;
    // }

}

void Lox::report(int line, const std::string &where, const std::string &message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message;
    had_error = true;
}

void Lox::runFile(std::string path) {
    auto input_text = readTextFile(std::move(path));
    run(input_text);
}

void Lox::runPrompt() {
    while (true) {
        std::cout << "> ";
        std::string current_line;
        getline(std::cin, current_line);
        if (current_line.empty())
            break;
        run(current_line);
        had_error = false;
    }
}

void Lox::error(int line, std::string message) {
    report(line, "", std::move(message));
}

void Lox::error(Token token, std::string message) {
    if (token.type == T_EOF) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }

}



