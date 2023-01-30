#pragma once

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include "types.h"
#include "token.h"

typedef std::variant<std::monostate, double, std::string> literal_type;
literal_type get_empty_literal();

class Scanner
{
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    char advance();

    void addToken(token_type);

    void addToken(token_type, literal_type);

    void scanToken();

    bool match(char);

    char peek();

    char peekNext();

    void string();

    static bool isDigit(char);

    static bool isAlpha(char);

    bool isAlphaNumeric(char);

    void number();

    void identifier();

    void multiline_comment();

    static std::unordered_map<std::string, token_type> keywords;

    bool isAtEnd();


public:
    Scanner(std::string source) : source(source){};

    std::vector<Token> scanTokens();
};