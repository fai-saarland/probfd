#ifndef LANGUAGE_TOKEN_H
#define LANGUAGE_TOKEN_H

#include <format>
#include <string>
#include <vector>

namespace language::parser {

enum class TokenType {
    OPENING_PARENTHESIS,
    CLOSING_PARENTHESIS,
    OPENING_BRACKET,
    CLOSING_BRACKET,
    COMMA,
    DOT,
    COLON,
    EQUALS,
    PLUS,
    MINUS,
    LET,
    LAMBDA,
    AS,
    IN,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_INTEGER,
    TYPE_FLOAT,
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

    Token(const std::string& content, TokenType type);
};

extern std::string token_type_name(TokenType token_type);
extern std::ostream& operator<<(std::ostream& out, TokenType token_type);
extern std::ostream& operator<<(std::ostream& out, const Token& token);
} // namespace downward::cli::parser

namespace std {

template <>
struct formatter<language::parser::TokenType> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const language::parser::TokenType& t, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", token_type_name(t));
    }
};

template <>
struct formatter<language::parser::Token> {
    bool text = false;

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        auto it = std::ranges::begin(ctx);
        const auto end = std::ranges::end(ctx);

        for (; it != end && *it != '}'; ++it) {
            switch (*it) {
            case 't': text = true; break;
            default: throw std::format_error("Could not parse specifier!");
            }
        }

        return it;
    }

    template <typename FormatContext>
    auto
    format(const language::parser::Token& token, FormatContext& ctx) const
    {
        if (text) { return std::format_to(ctx.out(), "{}", token.content); }

        return std::format_to(
            ctx.out(),
            "<Type: '{}', Value: '{}'>",
            token.type,
            token.content);
    }
};

} // namespace std
#endif
