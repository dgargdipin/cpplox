#pragma once

#include <utility>
#include <vector>
#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <memory>
#include "Stmt.hpp"
#include "token.h"

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
    int nested_loops = 0;
    int current = 0;

    unique_ptr<Expr> expression();

    unique_ptr<Expr> equality();

    unique_ptr<Expr> comparison();

    unique_ptr<Expr> term();

    unique_ptr<Expr> factor();

    unique_ptr<Expr> unary();

    unique_ptr<Expr> call();

    unique_ptr<Stmt> function(std::string kind);

    unique_ptr<Expr> finish_call(unique_ptr<Expr>);

    unique_ptr<Expr> primary();

    unique_ptr<Expr> comma();

    unique_ptr<Expr> ternary();

    unique_ptr<Expr> logical_or();

    unique_ptr<Expr> logical_and();

    unique_ptr<Expr> assignment();

    unique_ptr<Stmt> statement();

    unique_ptr<Return> return_statement();

    unique_ptr<If> if_statement();

    unique_ptr<While> while_statement();

    unique_ptr<Stmt> for_statement();

    unique_ptr<Stmt> declaration();

    unique_ptr<Var> var_declaration();

    unique_ptr<Print> print_statement();

    Lox::VecUniquePtr<Stmt> block();

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

    Lox::VecUniquePtr<Stmt> parseTokens();
};

