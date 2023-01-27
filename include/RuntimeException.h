//
// Created by Dipin Garg on 24-12-2022.
//

#ifndef LOX_RUNTIMEEXCEPTION_H
#define LOX_RUNTIMEEXCEPTION_H

#include <stdexcept>
#include "scanner.h"

namespace Lox {

    class RuntimeException: public std::exception {
    public:
    Token token;
    std::string message;
    RuntimeException(Token token, std::string message) : token(token),message(message){};
    const char *what();
};


} // Lox

#endif //LOX_RUNTIMEEXCEPTION_H
