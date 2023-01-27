#pragma once

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>

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
typedef std::variant<std::monostate, double, std::string> literal_type;
literal_type get_empty_literal();
class Token
{
public:
    int line;
    token_type type;

    literal_type literal;
    std::string lexeme;
    Token(token_type type, std::string lexeme, literal_type literal, int line) : type(type), lexeme(lexeme),
                                                                                 literal(literal), line(line){};
    //    std::string toString();
};

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