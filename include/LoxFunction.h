//
// Created by Dipin Garg on 30-01-2023.
//

#ifndef LOX_LOXFUNCTION_H
#define LOX_LOXFUNCTION_H

#include "Callable.h"
#include <optional>

namespace Lox {

    class LoxFunction : public Callable {
        FunctionExpr* function_definition;
        Environment *closure;
        std::optional<Token> name;
    public:
        LoxFunction(FunctionExpr* ptr, Environment *closure);

        LoxFunction(Token name,FunctionExpr* ptr, Environment *closure);

        Object call(Interpreter &interpreter, std::vector<Object> arguments);

        int arity();

        operator std::string() const {
            return "<fn " + (name ? (name->lexeme) : "anonymous") + ">";
        }

    };

} // Lox

#endif //LOX_LOXFUNCTION_H
