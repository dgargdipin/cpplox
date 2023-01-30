//
// Created by Dipin Garg on 18-01-2023.
//

#ifndef LOX_TYPES_H
#define LOX_TYPES_H

#include <any>
#include <vector>
#include <memory>

namespace Lox {
    template<typename T>
    class VecUniquePtr {
        std::vector<std::unique_ptr<T>> _vec;
    public:
        VecUniquePtr(std::vector<std::unique_ptr<T>> &vec) : _vec(std::move(vec)) {};

        VecUniquePtr(VecUniquePtr &&v) : _vec(std::move(v._vec)) {};

        VecUniquePtr(VecUniquePtr &v) : _vec(std::move(v._vec)) {};

        VecUniquePtr() {};

        std::vector<std::unique_ptr<T>> &get() {
            return _vec;
        }

        void push_back(std::unique_ptr<T> ptr) {
            _vec.push_back(std::move(ptr));
        }
    };

    typedef std::any Object;
}
enum token_type
{
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    QUESTION_MARK,
    COLON,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,
    T_BREAK,
    T_EOF,

};
#endif //LOX_TYPES_H
