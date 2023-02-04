//
// Created by Dipin Garg on 18-01-2023.
//

#include "Environment.h"
#include <iostream>
#include "LoxExceptions.h"
#include "token.h"

void Lox::Environment::define(std::string name, Object value) {
    values[name] = value;
}

Lox::Object Lox::Environment::get(Token name) {
//    std::cout << "ACCESSING " << name.lexeme << std::endl;

    if (values.count(name.lexeme)) {
        auto val = values.at(name.lexeme);
        if (!val.has_value()) {
            throw RuntimeException(name, "Can't access undefined variable");
        }
        return val;
    }
    if (enclosing)
        return enclosing->get(name);
    throw RuntimeException(name,
                           "Undefined variable '" + name.lexeme + "'.");
}

void Lox::Environment::assign(Token name, Object value) {
    if (values.count(name.lexeme)) {
        values[name.lexeme] = value;
        return;
    }
    if (enclosing)enclosing->assign(name, value);
    else throw RuntimeException(name, "Undefined variable " + name.lexeme + ".");
}

Lox::Environment::~Environment() {

}
