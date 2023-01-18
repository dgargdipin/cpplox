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

namespace Lox {

    class Environment {
        std::unordered_map<std::string, Object> values;
    public:

        void define(std::string name,Object value);
        Object get(Token name);


    };

} // Lox

#endif //LOX_ENVIRONMENT_H
