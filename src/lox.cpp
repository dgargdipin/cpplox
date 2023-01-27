#include <iostream>
#include <utility>
#include "lox.h"
#include "utils.h"
#include "parser.h"
#include <sysexits.h>
// #include "astprinter.h"
#include <interpreter.h>
#include<sstream>

std::string readTextFile(const std::string &path) {
    std::ifstream file(path);
    if (file.fail()) {
        Lox::error("Cannot open input file", path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();

}

namespace Lox {
    bool had_error = false;
    bool had_runtime_error = false;
    Interpreter interpreter;
}

void Lox::run(std::string input, bool print_expressions = false) {
    Scanner scanner(std::move(input));
    auto tokens = scanner.scanTokens();

    Parser parser(tokens);
    //    p.print();
    //    print();
    auto stmts = parser.parseTokens();
    if (had_error)
        return;
    // std::cout << ASTPrinter().print(std::move(expression));
    interpreter.interpret(stmts,print_expressions);
    //
    //    std::cout<<tokens.size()<<std::endl;

    // for (auto token : tokens)
    // {
    //     std::cout << token;
    // }
}

void Lox::report(int line, const std::string &where, const std::string &message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message<<std::endl;
    had_error = true;
}

void Lox::runFile(std::string path) {
    auto input_text = readTextFile(std::move(path));
    run(input_text);
    if (had_error)std::exit(EX_DATAERR);
    if (had_runtime_error)std::exit(EX_SOFTWARE);
}

void Lox::runPrompt() {
    while (true) {
        std::cout << "> ";
        std::string current_line;
        getline(std::cin, current_line);
        if (current_line.empty())
            break;
        run(current_line, true);
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

void Lox::runtime_error(Lox::RuntimeException &e) {
    std::cerr << e.what() << "\n[line " << e.token.line << "]\n";
    had_runtime_error = true;

}
