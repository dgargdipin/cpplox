//
// Created by Dipin Garg on 18-01-2023.
//

#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include<unordered_map>
#include<string>
#include "types.h"
#include "scanner.h"
#include "RuntimeException.h"
#include <memory>

namespace Lox {

    class Environment {
        Environment *enclosing;
        std::unordered_map<std::string, Object> values;
    public:
        Environment() = default;


        Environment(Environment *enclosing) : enclosing(enclosing) {};

        // set a variable, even create it if the name doesnt exist
        void define(std::string name, Object value);

        // update a varaible, throw error if name doesnt exist
        void assign(Token name, Object value);

        Object get(Token name);


    };

} // Lox

#endif //LOX_ENVIRONMENT_H
