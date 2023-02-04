//
// Created by Dipin Garg on 24-12-2022.
//

#ifndef LOX_LOXEXCEPTIONS_H
#define LOX_LOXEXCEPTIONS_H

#include <stdexcept>
#include "token.h"

namespace Lox {

    class RuntimeException : public std::exception {
    public:
        Token token;
        std::string message;

        RuntimeException(Token token, std::string message) : token(token), message(message) {};

        const char *what();
    };

    class BreakException : public std::exception {
    };

    class ReturnException : public std::exception {
    public:
        Object value;
        ReturnException(Object value) : value(value) {};
    };

} // Lox

#endif //LOX_LOXEXCEPTIONS_H
