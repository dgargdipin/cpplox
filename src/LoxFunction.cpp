//
// Created by Dipin Garg on 30-01-2023.
//

#include "LoxFunction.h"
#include "LoxExceptions.h"

namespace Lox {
    Object LoxFunction::call(Interpreter &interpreter, std::vector<Object> arguments) {
        Environment *env = new Environment(interpreter.global);
        for (int i = 0; i < declaration->params.size(); i++) {
            env->define(declaration->params[i].lexeme, arguments[i]);
        }
        try {
            interpreter.execute_block(declaration->body, env);
        }
        catch (const ReturnException &e) {
            delete env;
            return e.value;
        }
        delete env;
        return {};//return nil

    }

    int LoxFunction::arity() {
        return declaration->params.size();
    }
} // Lox