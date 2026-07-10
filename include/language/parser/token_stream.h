#ifndef LANGUAGE_PARSER_TOKEN_STREAM_H
#define LANGUAGE_PARSER_TOKEN_STREAM_H

#include <format>
#include <string>
#include <vector>

namespace language {
class Context;
}

namespace language::parser {

enum class TokenType {
    OPENING_PARENTHESIS,
    CLOSING_PARENTHESIS,
    OPENING_BRACKET,
    CLOSING_BRACKET,
    COMMA,
    EQUALS,
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

    Token(std::string content, TokenType type);
};

class TokenStream {
    std::vector<Token> tokens;
    int pos;

public:
    explicit TokenStream(std::vector<Token>&& tokens);

    bool has_tokens(int n) const;

    Token peek(const Context& context, int n = 0) const;

    Token pop(const Context& context);

    Token pop(const Context& context, TokenType expected_type);

    int get_position() const;

    int size() const;

    std::string str(int from, int to) const;
};

extern std::string token_type_name(TokenType token_type);

extern std::ostream& operator<<(std::ostream& out, TokenType token_type);
extern std::ostream& operator<<(std::ostream& out, const Token& token);
} // namespace language::parser

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
