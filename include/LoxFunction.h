//
// Created by Dipin Garg on 30-01-2023.
//

#ifndef LOX_LOXFUNCTION_H
#define LOX_LOXFUNCTION_H

#include "Callable.h"

namespace Lox {

    class LoxFunction : public Callable {
        Function* declaration;
    public:
        LoxFunction(Function *ptr) : declaration(ptr) {};

        Object call(Interpreter &interpreter, std::vector<Object> arguments);

        int arity();

        std::string to_string(){
            return "<fn " + declaration->name.lexeme + ">";
        }
    };

} // Lox

#endif //LOX_LOXFUNCTION_H
