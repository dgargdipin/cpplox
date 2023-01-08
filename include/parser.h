#pragma once

#include "scanner.h"
#include <utility>
#include <vector>
#include "Expr.hpp"
#include <initializer_list>
#include <stdexcept>
#include "lox.h"
#include <iostream>
#include <memory>

using std::unique_ptr;
using std::vector;

class ParseError : public std::exception {
public:
    ParseError() = default;
};

class HandledParseError {
public:
    Token token;
    std::string message;

    HandledParseError(Token token, std::string message) : token(token), message(message) {};
};

template<typename T>
class Parser {
    vector<Token> tokens;
    vector<HandledParseError> handled_parse_errors;
    int current = 0;

    unique_ptr<Expr<T>> expression();

    unique_ptr<Expr<T>> equality();

    unique_ptr<Expr<T>> comparison();

    unique_ptr<Expr<T>> term();

    unique_ptr<Expr<T>> factor();

    unique_ptr<Expr<T>> unary();

    unique_ptr<Expr<T>> primary();

    unique_ptr<Expr<T>> comma();

    unique_ptr<Expr<T>> ternary();

    bool match(std::initializer_list<token_type>);

    template<class DstType, class SrcType>
    bool IsType(const SrcType *src) {
        return dynamic_cast<const DstType *>(src) != nullptr;
    }

    void check_missing_expr(Expr<T> *expr, std::string error_message);

    void check_invalid_token(token_type token, Token previous, std::string error_message);

    bool check(token_type);

    bool isAtEnd();

    Token advance();

    Token previous();

    Token peek();

    Token consume(token_type type, std::string message);

    ParseError error(Token token, std::string message);

    void synchronise();

public:
    explicit Parser(vector<Token> tokens) : tokens(std::move(tokens)) {};

    unique_ptr<Expr<T>> parseTokens();
};

template<typename T>
void Parser<T>::check_missing_expr(Expr<T> *expr, std::string error_message) {
    if (IsType<Nothing<T> >(expr)) {
        handled_parse_errors.push_back(HandledParseError(previous(), error_message));
    };
}

template<typename T>
void Parser<T>::check_invalid_token(token_type token, Token previous, std::string error_message) {
    if (previous.type == token) {
        handled_parse_errors.push_back(HandledParseError(previous, error_message));
    }
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::expression() {
    return comma();
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::comma() {

    auto expr = ternary();
    while (match({COMMA})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = ternary();
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
    }
    return expr;
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::ternary() {
    auto expr = equality();
    if (match({QUESTION_MARK})) {
        auto if_match = comma();
        consume(COLON, "EXPECTED COLON");
        auto if_not_match = ternary();
        return std::make_unique<Ternary<T> >(expr, if_match, if_not_match);
    }
    return expr;
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::equality() {
    auto expr = comparison();
    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = comparison();

        // expr = new Binary<T>(expr, operator_token, right);
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
    }
    return expr;
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::comparison() {
    auto expr = term();
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = term();
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
        // expr = new Binary<T>(expr, operator_token, right);
    }
    return expr;
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::term() {
    auto expr = factor();
    while (match({MINUS, PLUS})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = factor();
        // expr = new Binary<T>(expr, operator_token, right);
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
    }
    return expr;
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::factor() {
    auto expr = unary();
    while (match({SLASH, STAR})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = unary();
        // expr = new Binary<T>(expr, operator_token, right);
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
    }
    return expr;
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::unary() {
    if (match({BANG, MINUS})) {
        Token operator_token = previous();
        auto right = unary();
        return std::make_unique<Unary<T> >(operator_token, right);
        // return new Unary<T>(operator_token, right);
    }
    return primary();
}

template<typename T>
unique_ptr<Expr<T>> Parser<T>::primary() {
    if (match({FALSE}))
        return std::make_unique<Literal<T> >(false);
    if (match({TRUE}))
        return std::make_unique<Literal<T> >(true);
    if (match({NIL}))
        return std::make_unique<Literal<T> >(std::any{});
    check_invalid_token(DOT, peek(), "Values cannot begin with a dot.");
    if (match({NUMBER})) {
        literal_type literal = previous().literal;
        double val = std::get<double>(literal);
        return std::make_unique<Literal<T> >(val);
    }

    if (match({STRING})) {
        literal_type literal = previous().literal;
        std::string val = std::get<std::string>(literal);
        return std::make_unique<Literal<T> >(val);
    }
    if (match({LEFT_PAREN})) {
        auto expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression");
        return std::make_unique<Grouping<T> >(expr);
    }
//    throw error(peek(), "Expect Expression");
    return std::make_unique<Nothing<T> >("Placeholder");
}

//T->T?T:T,T
template<typename T>
bool Parser<T>::match(std::initializer_list<token_type> types) {
    for (auto type: types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

template<typename T>
bool Parser<T>::check(token_type t) {
    if (isAtEnd())
        return false;
    return peek().type == t;
}

template<typename T>
Token Parser<T>::advance() {
    if (!isAtEnd())
        current++;
    return previous();
}

template<typename T>
Token Parser<T>::previous() {
    return tokens.at(current - 1);
}

template<typename T>
Token Parser<T>::peek() {
    return tokens.at(current);
}

template<typename T>
bool Parser<T>::isAtEnd() {
    return peek().type == EOF;
}

template<typename T>
ParseError Parser<T>::error(Token token, std::string message) {
    Lox::error(std::move(token), std::move(message));
    return {};
}

template<typename T>
Token Parser<T>::consume(token_type type, std::string message) {
    if (check(type)) {
        return advance();
    }
    throw error(peek(), message);
}

template<typename T>
void Parser<T>::synchronise() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == SEMICOLON)
            return;
        switch (peek().type) {
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case RETURN:
                return;
        }
        advance();
    }
}

// template<typename T>
// void Parser<T>::parseTokens() {
//     try{
//         return;
//         return expression();
//     }
//     catch (ParseError error) {
//         return;
//     }
// }
template<typename T>
unique_ptr<Expr<T>> Parser<T>::parseTokens() {


    auto res = this->expression();
    for (auto &error: handled_parse_errors) {
        Lox::error(error.token, error.message);
    }
    return res;

}
