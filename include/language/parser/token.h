#ifndef LANGUAGE_PARSER_TOKEN_H
#define LANGUAGE_PARSER_TOKEN_H

#include <string>

namespace language::parser {

enum class TokenType {
    OPENING_PARENTHESIS,
    CLOSING_PARENTHESIS,
    OPENING_BRACKET,
    CLOSING_BRACKET,
    COMMA,
    EQUALS,
    PLUS,
    MINUS,
    LET,
    AS,
    IN,
    TRUE,
    FALSE,
    STRING,
    INTEGER,
    FLOAT,
    IDENTIFIER
};

struct Token {
    std::string content;
    TokenType type;
};

extern std::string token_type_name(TokenType token_type);

extern std::ostream& operator<<(std::ostream& out, TokenType token_type);
extern std::ostream& operator<<(std::ostream& out, const Token& token);
} // namespace language::parser

#include <format>
#include <vector>

template <>
struct std::hash<language::parser::TokenType> {
    size_t operator()(const language::parser::TokenType& t) const noexcept
    {
        return static_cast<size_t>(t);
    }
};

template <>
struct std::formatter<language::parser::TokenType> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(
        const language::parser::TokenType& token_type,
        std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{}", token_type_name(token_type));
    }
};

template <>
struct std::formatter<language::parser::Token> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto
    format(const language::parser::Token& token, std::format_context& ctx) const
    {
        return std::format_to(
            ctx.out(),
            "<Type: '{}', Value: '{}'>",
            token.type,
            token.content);
    }
};

#endif
