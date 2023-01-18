//
// Created by Dipin Garg on 18-01-2023.
//

#include "Environment.h"

namespace Lox {
    void Environment::define(std::string name, Object value) {
        values[name] = value;
    }

    Object Environment::get(Token name) {
        if (values.count(name.lexeme)) {
            return values.at(name.lexeme);
        }

        throw new RuntimeException(name,
                                   "Undefined variable '" + name.lexeme + "'.");
    }

    void Environment::assign(Token name, Object value) {
        if(values.count(name.lexeme)){
            values[name.lexeme]=value;
            return;
        }
        throw RuntimeException(name,"Undefined variable '\" + name.lexeme + \"'.");
    }
} // Lox