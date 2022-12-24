//
// Created by Dipin Garg on 24-12-2022.
//

#ifndef LOX_RUNTIMEEXCEPTION_H
#define LOX_RUNTIMEEXCEPTION_H

#include <stdexcept>
#include "scanner.h"

namespace Lox {

    class RuntimeException: public std::runtime_error {
    public:
    Token token;
    RuntimeException(Token token, std::string message) : token(token), std::runtime_error(message){};
};


} // Lox

#endif //LOX_RUNTIMEEXCEPTION_H
