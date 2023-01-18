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
#include "Stmt.hpp"

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


class Parser {
    vector<Token> tokens;
    vector<HandledParseError> handled_parse_errors;
    int current = 0;

    unique_ptr<Expr> expression();

    unique_ptr<Expr> equality();

    unique_ptr<Expr> comparison();

    unique_ptr<Expr> term();

    unique_ptr<Expr> factor();

    unique_ptr<Expr> unary();

    unique_ptr<Expr> primary();

    unique_ptr<Expr> comma();

    unique_ptr<Expr> ternary();

    unique_ptr<Stmt> statement();

    unique_ptr<Stmt> declaration();

    unique_ptr<Var> var_declaration();

    unique_ptr<Print> print_statement();

    unique_ptr<Expression> expression_statement();

    bool match(std::initializer_list<token_type>);

    template<class DstType, class SrcType>
    bool IsType(const SrcType *src) {
        return dynamic_cast<const DstType *>(src) != nullptr;
    }

    void check_missing_expr(Expr *expr, std::string error_message);

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

    std::vector<unique_ptr<Stmt> > parseTokens();
};


void Parser::check_missing_expr(Expr *expr, std::string error_message) {
    if (IsType<Nothing>(expr)) {
        handled_parse_errors.push_back(HandledParseError(previous(), error_message));
    };
}


void Parser::check_invalid_token(token_type token, Token previous, std::string error_message) {
    if (previous.type == token) {
        handled_parse_errors.push_back(HandledParseError(previous, error_message));
    }
}


unique_ptr<Expr> Parser::expression() {
    return comma();
}


unique_ptr<Expr> Parser::comma() {

    auto expr = ternary();
    while (match({COMMA})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = ternary();
        expr = std::make_unique<Binary>(expr, operator_token, right);
    }
    return expr;
}


unique_ptr<Expr> Parser::ternary() {
    auto expr = equality();
    if (match({QUESTION_MARK})) {
        auto if_match = comma();
        consume(COLON, "EXPECTED COLON");
        auto if_not_match = ternary();
        return std::make_unique<Ternary>(expr, if_match, if_not_match);
    }
    return expr;
}


unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = comparison();

        // expr = new Binary(expr, operator_token, right);
        expr = std::make_unique<Binary>(expr, operator_token, right);
    }
    return expr;
}


unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = term();
        expr = std::make_unique<Binary>(expr, operator_token, right);
        // expr = new Binary(expr, operator_token, right);
    }
    return expr;
}


unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match({MINUS, PLUS})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = factor();
        // expr = new Binary(expr, operator_token, right);
        expr = std::make_unique<Binary>(expr, operator_token, right);
    }
    return expr;
}


unique_ptr<Expr> Parser::factor() {
    auto expr = unary();
    while (match({SLASH, STAR})) {
        check_missing_expr(expr.get(), "Binary operators must have a left and a right operand");
        Token operator_token = previous();
        auto right = unary();
        // expr = new Binary(expr, operator_token, right);
        expr = std::make_unique<Binary>(expr, operator_token, right);
    }
    return expr;
}


unique_ptr<Expr> Parser::unary() {
    if (match({BANG, MINUS})) {
        Token operator_token = previous();
        auto right = unary();
        return std::make_unique<Unary>(operator_token, right);
        // return new Unary(operator_token, right);
    }
    return primary();
}


unique_ptr<Expr> Parser::primary() {
    if (match({FALSE}))
        return std::make_unique<Literal>(false);
    if (match({TRUE}))
        return std::make_unique<Literal>(true);
    if (match({NIL}))
        return std::make_unique<Literal>(std::any{});
    check_invalid_token(DOT, peek(), "Values cannot begin with a dot.");
    if (match({NUMBER})) {
        literal_type literal = previous().literal;
        double val = std::get<double>(literal);
        return std::make_unique<Literal>(val);
    }

    if (match({STRING})) {
        literal_type literal = previous().literal;
        std::string val = std::get<std::string>(literal);
        return std::make_unique<Literal>(val);
    }
    if (match({LEFT_PAREN})) {
        auto expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression");
        return std::make_unique<Grouping>(expr);
    }
    if (match({IDENTIFIER})) {
        return std::make_unique<Variable>(previous());
    }
    throw error(peek(), "Expect Expression");
//    advance(); // advance curr pointer to next so that rest of expr can be parsed because this fn will return Nothing
//    return std::make_unique<Nothing >("Placeholder");
}

//T->T?T:T,T

bool Parser::match(std::initializer_list<token_type> types) {
    for (auto type: types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}


bool Parser::check(token_type t) {
    if (isAtEnd())
        return false;
    return peek().type == t;
}


Token Parser::advance() {
    if (!isAtEnd())
        current++;
    return previous();
}


Token Parser::previous() {
    return tokens.at(current - 1);
}


Token Parser::peek() {
    return tokens.at(current);
}


bool Parser::isAtEnd() {
    return peek().type == T_EOF;
}


ParseError Parser::error(Token token, std::string message) {
    Lox::error(std::move(token), std::move(message));
    return {};
}


Token Parser::consume(token_type type, std::string message) {
    if (check(type)) {
        return advance();
    }
    throw error(peek(), message);
}


void Parser::synchronise() {
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

//
// void Parser::parseTokens() {
//     try{
//         return;
//         return expression();
//     }
//     catch (ParseError error) {
//         return;
//     }
// }

std::vector<unique_ptr<Stmt> > Parser::parseTokens() {

    std::vector<unique_ptr<Stmt> > statements;
    while (!isAtEnd()) {
        statements.emplace_back(declaration());
    }
//    auto res = this->expression();
    for (auto &error: handled_parse_errors) {
        Lox::error(error.token, error.message);
    }
    return statements;

}

unique_ptr<Stmt> Parser::statement() {
    if (match({PRINT})) {
        return print_statement();
    }
    return expression_statement();
}

unique_ptr<Print> Parser::print_statement() {
    auto value = expression();
    consume(SEMICOLON, "Expected ';' after value");
    return std::make_unique<Print>(value);
}

unique_ptr<Expression> Parser::expression_statement() {
    auto value = expression();
    consume(SEMICOLON, "Expected ';' after expression");
    return std::make_unique<Expression>(value);
}

unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({VAR})) {
            return var_declaration();
        }
        return statement();
    }
    catch (ParseError &e) {
        synchronise();
        return nullptr;
    }
}

unique_ptr<Var> Parser::var_declaration() {
    Token name = consume(IDENTIFIER, "Expect variable name");
    std::unique_ptr<Expr> initializer;
    if (match({EQUAL})) {
        initializer = expression();
    }
    consume(SEMICOLON, "Expect ';' after variable declaration");
    return std::make_unique<Var>(name, initializer);
}

