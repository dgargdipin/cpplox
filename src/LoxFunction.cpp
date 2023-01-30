//
// Created by Dipin Garg on 30-01-2023.
//

#include "LoxFunction.h"

namespace Lox {
    Object LoxFunction::call(Interpreter &interpreter, std::vector<Object> arguments) {
        Environment *env = new Environment(interpreter.global);
        for (int i = 0; i < declaration->params.size(); i++) {
            env->define(declaration->params[i].lexeme, arguments[i]);
        }
        interpreter.execute_block(declaration->body, env);
        delete env;

    }

    int LoxFunction::arity() {
        return declaration->params.size();
    }
} // Lox