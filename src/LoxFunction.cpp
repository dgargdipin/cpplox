//
// Created by Dipin Garg on 30-01-2023.
//

#include "LoxFunction.h"
#include "LoxExceptions.h"

namespace Lox {
    Object LoxFunction::call(Interpreter &interpreter, std::vector<Object> arguments) {
        Environment *env = new Environment(closure);
        for (int i = 0; i < declaration->params.size(); i++) {
            env->define(declaration->params[i].lexeme, arguments[i]);
        }
        try {
            interpreter.execute_block(declaration->body, env);
        }
        catch (const ReturnException &e) {
            return e.value;
        }
        return {};//return nil

    }

    int LoxFunction::arity() {
        return declaration->params.size();
    }

    LoxFunction::LoxFunction(Function *ptr, Environment *closure) : declaration(ptr), closure(closure) {

    }
} // Lox