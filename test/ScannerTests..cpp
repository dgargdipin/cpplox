#include<gtest/gtest.h>
#include "scanner.h"

//
// Created by Dipin Garg on 25-12-2022.
//
bool checkLiteralsEqual(token_type type, const literal_type &el, const literal_type &l) {
    if (el.index() != l.index()) {
        return false;
    }

    switch (type) {
        case STRING:
            return std::get<std::string>(el) == std::get<std::string>(l);
        case NUMBER:
            return std::get<double>(el) == std::get<double>(l);
        default:
            return true;
    }
}

void checkTokensEqual(const std::vector<Token> &ets, const std::vector<Token> &ts) {
    ASSERT_EQ(ets.size(), ts.size());
    for (std::size_t i = 0; i < ts.size(); ++i) {
        EXPECT_EQ(ets[i].type, ts[i].type) << "Token types differ at index " << i;
        EXPECT_EQ(ets[i].lexeme, ts[i].lexeme) << "Token lexemes differ at index " << i;
        EXPECT_EQ(ets[i].line, ts[i].line) << "Token lines differ at index " << i;
        EXPECT_TRUE(checkLiteralsEqual(ets[i].type, ets[i].literal, ts[i].literal))
                            << "Literals differ at index " << i;
    }
}

TEST(ScannerTests, Basic) {
    const auto testScript = R"(print "Hello, world" 42)";
    Scanner scanner{testScript};
    const auto tokens = scanner.scanTokens();
    /* clang-format off */
    std::vector<Token> expectedTokens = {
            Token{PRINT, "print", get_empty_literal(), 1},
            Token{STRING, "\"Hello, world\"", std::string{"Hello, world"}, 1},
            Token{NUMBER, "42", 42.0, 1},
            Token{T_EOF, "", get_empty_literal(), 1},
    };
    checkTokensEqual(expectedTokens, tokens);

}

TEST(ScannerTests, Comment) {
    const auto testScript = R"(// this is a comment
print "Hello, world" // another comment
/*asdasdsad print*/123
)";

    Scanner scanner{testScript};
    const auto tokens = scanner.scanTokens();
    /* clang-format off */
    std::vector<Token> expectedTokens = {
            Token{PRINT, "print", get_empty_literal(), 2},
            Token{STRING, "\"Hello, world\"", std::string{"Hello, world"}, 2},

            Token{NUMBER, "123", 123.0, 3},
            Token{T_EOF, "", get_empty_literal(), 4},
    };
    /* clang-format on */

    checkTokensEqual(expectedTokens, tokens);
}

TEST(ScannerTests, Expression) {
    const auto testScript = R"(var x = 2 + 2)";

    Scanner scanner{testScript};
    const auto tokens = scanner.scanTokens();
    /* clang-format off */
    std::vector<Token> expectedTokens = {
            Token{VAR, "var", get_empty_literal(), 1},
            Token{IDENTIFIER, "x", get_empty_literal(), 1},
            Token{EQUAL, "=", get_empty_literal(), 1},
            Token{NUMBER, "2", 2., 1},
            Token{PLUS, "+", get_empty_literal(), 1},
            Token{NUMBER, "2", 2., 1},
            Token{T_EOF, "", get_empty_literal(), 1},
    };
    /* clang-format on */

    checkTokensEqual(expectedTokens, tokens);
}
