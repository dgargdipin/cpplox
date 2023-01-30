//
// Created by Dipin Garg on 24-12-2022.
//

#ifndef LOX_RUNTIMEEXCEPTION_H
#define LOX_RUNTIMEEXCEPTION_H

#include <stdexcept>
#include "token.h"

namespace Lox {

    class RuntimeException: public std::exception {
    public:
    Token token;
    std::string message;
    RuntimeException(Token token, std::string message) : token(token),message(message){};
    const char *what();
};

class BreakException: public std::exception{
};

} // Lox

#endif //LOX_RUNTIMEEXCEPTION_H
