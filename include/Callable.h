//
// Created by Dipin Garg on 30-01-2023.
//

#ifndef LOX_CALLABLE_H
#define LOX_CALLABLE_H

#include "interpreter.h"

namespace Lox {
    class Interpreter;

    class Callable {
    public:
        virtual Object call(Interpreter &interpreter, std::vector<Object> arguments) = 0;

        virtual ~Callable() = default;

        virtual int arity() = 0;
    };

} // Lox

#endif //LOX_CALLABLE_H
