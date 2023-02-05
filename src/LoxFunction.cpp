//
// Created by Dipin Garg on 30-01-2023.
//

#include "LoxFunction.h"
#include "LoxExceptions.h"

namespace Lox {
    Object LoxFunction::call(Interpreter &interpreter, std::vector<Object> arguments) {
        Environment *env = new Environment(closure);
        for (int i = 0; i < function_definition->params.size(); i++) {
            env->define(function_definition->params[i].lexeme, arguments[i]);
        }
        try {
            interpreter.execute_block(function_definition->body, env);
        }
        catch (const ReturnException &e) {
            return e.value;
        }
        return {};//return nil

    }

    int LoxFunction::arity() {
        return function_definition->params.size();
    }

    LoxFunction::LoxFunction(FunctionExpr *ptr, Environment *closure) : function_definition(
            ptr),
                                                                        closure(closure) {}

    LoxFunction::LoxFunction(Token name_token, FunctionExpr *ptr, Environment *closure) : function_definition(
            ptr),
                                                                                          closure(closure),
                                                                                          name(name_token) {
    }
} // Lox