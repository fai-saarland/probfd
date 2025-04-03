
#ifndef PROBFD_UTILS_JSON_TOKENIZER_H
#define PROBFD_UTILS_JSON_TOKENIZER_H

#include <iostream>
#include <vector>

namespace probfd::json {

enum class TokenType {
    END_OF_FILE,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    TRUE,
    FALSE,
    NUL,
    INTEGER,
    FLOAT,
    STRING,
    COLON,
    COMMA,
    /*
    INFTY,
    NOT_A_NUMBER
    */
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;
};

std::vector<Token> tokenize(std::istream& is);

} // namespace probfd::json

#endif
