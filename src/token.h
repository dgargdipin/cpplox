//
// Created by Dipin Garg on 30-01-2023.
//

#ifndef LOX_TOKEN_H
#define LOX_TOKEN_H

#include "types.h"
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>

class Token
{
public:
    int line;
    token_type type;

    std::variant<std::monostate, double, std::string> literal;
    std::string lexeme;
    Token(token_type type, std::string lexeme, std::variant<std::monostate, double, std::string> literal, int line) : type(type), lexeme(lexeme),
                                                                                                                      literal(literal), line(line){};
    //    std::string toString();
};

#endif //LOX_TOKEN_H
