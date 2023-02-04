//
// Created by Dipin Garg on 24-12-2022.
//

#include "LoxExceptions.h"

namespace Lox {

    const char *RuntimeException::what() {
        return message.c_str();
    }
} // Lox