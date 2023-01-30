#include <iostream>
#include "lox.h"
#include "logger.h"
#include "token.h"
#include "scanner.h"
loglevel_e loglevel = logERROR;
std::unordered_map<std::string, token_type> Scanner::keywords = {{"and",    AND},
                                                                 {"class",  CLASS},
                                                                 {"else",   ELSE},
                                                                 {"false",  FALSE},
                                                                 {"for",    FOR},
                                                                 {"fun",    FUN},
                                                                 {"if",     IF},
                                                                 {"nil",    NIL},
                                                                 {"or",     OR},
                                                                 {"print",  PRINT},
                                                                 {"return", RETURN},
                                                                 {"super",  SUPER},
                                                                 {"this",   THIS},
                                                                 {"true",   TRUE},
                                                                 {"var",    VAR},
                                                                 {"while",  WHILE},
                                                                 {"break",  T_BREAK},
};

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.emplace_back(token_type::T_EOF, "", get_empty_literal(), line);
    return tokens;
}

bool Scanner::isAtEnd() {
    return current >= source.size();
}

char Scanner::advance() {
    return source[current++];
}

void Scanner::addToken(token_type t) {
    addToken(t, get_empty_literal());
}

void Scanner::addToken(token_type t, literal_type literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(t, text, literal, line);
}

bool Scanner::match(char expected) {
    if (isAtEnd())
        return false;
    if (source[current] != expected)
        return false;
    current++;
    return true;
}

char Scanner::peek() {
    if (isAtEnd())
        return '\0';
    return source[current];
}

char Scanner::peekNext() {
    if (current + 1 >= source.size())
        return '\0';
    return source[current + 1];
}

void Scanner::multiline_comment() {
    bool found = false;
    while (!found && !isAtEnd()) {
        char c = advance();
        if (c == '\n')line++;
        else if (c == '*' && peek() == '/')found = true;
    }
    advance();
}

void Scanner::scanToken() {
    char c = advance();
    switch (c) {
        case '(':
            addToken(LEFT_PAREN);
            break;
        case ')':
            addToken(RIGHT_PAREN);
            break;
        case '{':
            addToken(LEFT_BRACE);
            break;
        case '}':
            addToken(RIGHT_BRACE);
            break;
        case ',':
            addToken(COMMA);
            break;
        case '.':
            addToken(DOT);
            break;
        case '-':
            addToken(MINUS);
            break;
        case '+':
            addToken(PLUS);
            break;
        case ';':
            addToken(SEMICOLON);
            break;
        case '*':
            addToken(STAR);
            break;
        case '!':
            addToken(match('=') ? BANG_EQUAL : BANG);
            break;
        case '=':
            addToken(match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            addToken(match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            addToken(match('=') ? GREATER_EQUAL : GREATER);
            break;
        case '?':
            addToken(QUESTION_MARK);
            break;
        case ':':
            addToken(COLON);
            break;
        case '/': {
            if (match('/')) {
                while (!isAtEnd() && peek() != '\n')
                    advance();
            } else if (match('*')) {
                multiline_comment();
            } else {
                addToken(SLASH);
            }
        }
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            line++;
            break;
        case '"':
            string();
            break;
        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                Lox::error(line, "Unexpected Character.");
            }
            break;
    }
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

void Scanner::number() {
    while (isDigit(peek()))
        advance();
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek()))
            advance();
    }
    addToken(NUMBER, stod(source.substr(start, current - start)));
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Scanner::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Scanner::identifier() {
    while (isAlphaNumeric(peek()))
        advance();
    auto text = source.substr(start, current - start);
    auto itr = keywords.find(text);
    token_type token_t = IDENTIFIER;
    if (itr != keywords.end()) {
        token_t = itr->second;
    }
    addToken(token_t);
}

void Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
            line++;
        advance();
    }
    if (isAtEnd()) {
        Lox::error(line, "Unterminated String.");
    }
    advance();
    auto value = source.substr(start + 1, current - start - 2);
    addToken(STRING, value);
}

literal_type get_empty_literal() {
    return std::monostate{};
}