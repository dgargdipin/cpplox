//
// Created by Dipin Garg on 30-01-2023.
//

#include "Clock.h"
#include <chrono>

uint64_t timeSinceEpochMillisec() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

Lox::Object Lox::Clock::call(Interpreter &interpreter, std::vector<Object> arguments) {
    return (double) timeSinceEpochMillisec() / 1000.0;
}

int Lox::Clock::arity() {
    return 0;
}

std::string Lox::Clock::to_string() {
    return "<native fn>";
}
