
#ifndef PROBFD_JSON_TOKENIZE_H
#define PROBFD_JSON_TOKENIZE_H

#include <format>
#include <iostream>
#include <stdexcept>
#include <vector>

// The MSVC standard library defines this...
#undef TRUE
#undef FALSE

namespace probfd::json {

class TokenizationError final : public std::runtime_error {
public:
    template <typename... Args>
    explicit TokenizationError(
        std::format_string<Args...> fmt,
        Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

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
