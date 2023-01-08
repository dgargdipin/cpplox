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

class ParseError : public std::exception
{
public:
    ParseError() = default;
};

template <typename T>
class ParserClass
{
    vector<Token> tokens;
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
    bool check(token_type);
    bool isAtEnd();
    Token advance();
    Token previous();
    Token peek();
    Token consume(token_type type, std::string message);
    ParseError error(Token token, std::string message);
    void synchronise();

public:
    explicit ParserClass(vector<Token> tokens) : tokens(std::move(tokens)){};

    unique_ptr<Expr<T>> parseTokens();
};

template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::expression()
{
    return comma();
}

template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::comma()
{
    auto expr=ternary();
    while(match({COMMA})){
        Token operator_token=previous();
        auto right=ternary();
        expr=std::make_unique<Binary<T> >(expr,operator_token,right);
    }
    return expr;
}
//TERNARY->EQUALITY|EXPRESSION?EXPRESSION:EXPRESSION?EXPRESSION:EXPRESSION
template<typename T>
unique_ptr<Expr<T>> ParserClass<T>::ternary() {
    auto expr=equality();
    if(match({QUESTION_MARK})){
        auto if_match=comma();
        consume(COLON,"EXPECTED COLON");
        auto if_not_match=ternary();
        return std::make_unique<Ternary<T> >(expr,if_match,if_not_match);
    }
    return expr;
}
//TERNARY->EQUALITY||EXPRESSION?EXPRESSION:EXPRESSION
template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::equality()
{
    auto expr = comparison();
    while (match({BANG_EQUAL, EQUAL_EQUAL}))
    {
        Token operator_token = previous();
        auto right = comparison();

        // expr = new Binary<T>(expr, operator_token, right);
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
    }
    return expr;
}

template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::comparison()
{
    auto expr = term();
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL}))
    {
        Token operator_token = previous();
        auto right = term();
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
        // expr = new Binary<T>(expr, operator_token, right);
    }
    return expr;
}

template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::term()
{
    auto expr = factor();
    while (match({MINUS, PLUS}))
    {
        Token operator_token = previous();
        auto right = factor();
        // expr = new Binary<T>(expr, operator_token, right);
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
    }
    return expr;
}

template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::factor()
{
    auto expr = unary();
    while (match({SLASH, STAR}))
    {
        Token operator_token = previous();
        auto right = unary();
        // expr = new Binary<T>(expr, operator_token, right);
        expr = std::make_unique<Binary<T> >(expr, operator_token, right);
    }
    return expr;
}

template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::unary()
{
    if (match({BANG, MINUS}))
    {
        Token operator_token = previous();
        auto right = unary();
        return std::make_unique<Unary<T> >(operator_token, right);
        // return new Unary<T>(operator_token, right);
    }
    return primary();
}

template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::primary()
{
    if (match({FALSE}))
        return std::make_unique<Literal<T> >(false);
    if (match({TRUE}))
        return std::make_unique<Literal<T> >(true);
    if (match({NIL}))
        return std::make_unique<Literal<T> >(std::any{});

    if (match({NUMBER}))
    {
        literal_type literal = previous().literal;
        double val = std::get<double>(literal);
        return std::make_unique<Literal<T> >(val);
    }

    if (match({STRING}))
    {
        literal_type literal = previous().literal;
        std::string val = std::get<std::string>(literal);
        return std::make_unique<Literal<T> >(val);
    }
    if (match({LEFT_PAREN}))
    {
        auto expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression");
        return std::make_unique<Grouping<T> >(expr);
    }
    throw error(peek(), "Expect Expression");
}
//T->T?T:T,T
template <typename T>
bool ParserClass<T>::match(std::initializer_list<token_type> types)
{
    for (auto type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

template <typename T>
bool ParserClass<T>::check(token_type t)
{
    if (isAtEnd())
        return false;
    return peek().type == t;
}

template <typename T>
Token ParserClass<T>::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

template <typename T>
Token ParserClass<T>::previous()
{
    return tokens.at(current - 1);
}

template <typename T>
Token ParserClass<T>::peek()
{
    return tokens.at(current);
}

template <typename T>
bool ParserClass<T>::isAtEnd()
{
    return peek().type == EOF;
}

template <typename T>
ParseError ParserClass<T>::error(Token token, std::string message)
{
    Lox::error(std::move(token), std::move(message));
    return {};
}

template <typename T>
Token ParserClass<T>::consume(token_type type, std::string message)
{
    if (check(type))
    {
        return advance();
    }
    throw error(peek(), message);
}

template <typename T>
void ParserClass<T>::synchronise()
{
    advance();
    while (!isAtEnd())
    {
        if (previous().type == SEMICOLON)
            return;
        switch (peek().type)
        {
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
// void ParserClass<T>::parseTokens() {
//     try{
//         return;
//         return expression();
//     }
//     catch (ParseError error) {
//         return;
//     }
// }
template <typename T>
unique_ptr<Expr<T>> ParserClass<T>::parseTokens()
{

    try
    {
        return this->expression();
    }
    catch (ParseError &e)
    {
        return nullptr;
    }
}
