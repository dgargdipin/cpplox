//
// Created by Dipin Garg on 30-01-2023.
//

#ifndef LOX_CLOCK_H
#define LOX_CLOCK_H

#include "Callable.h"
#include "types.h"

namespace Lox {
    class Clock : public Callable {
        Object call(Interpreter &interpreter, std::vector<Object> arguments);

        int arity();

        std::string to_string();
    };

}

#endif //LOX_CLOCK_H
