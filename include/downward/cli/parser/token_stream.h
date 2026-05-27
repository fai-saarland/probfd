#ifndef PARSER_TOKEN_STREAM_H
#define PARSER_TOKEN_STREAM_H

#include <format>
#include <string>
#include <vector>

// The MSVC standard library defines this...
#undef IN

namespace downward::utils {
class Context;
}

namespace downward::cli::parser {

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

class TokenStream {
    std::vector<Token> tokens;
    int pos;

public:
    explicit TokenStream(std::vector<Token>&& tokens);

    bool has_tokens(int n) const;
    Token peek(const utils::Context& context, int n = 0) const;
    Token pop(const utils::Context& context);
    Token pop(const utils::Context& context, TokenType expected_type);

    int get_position() const;
    int size() const;
    std::string str(int from, int to) const;
};

extern std::string token_type_name(TokenType token_type);
extern std::ostream& operator<<(std::ostream& out, TokenType token_type);
extern std::ostream& operator<<(std::ostream& out, const Token& token);
} // namespace downward::cli::parser

namespace std {
template <>
struct hash<downward::cli::parser::TokenType> {
    size_t operator()(const downward::cli::parser::TokenType& t) const noexcept
    {
        return size_t(t);
    }
};

template <>
struct formatter<downward::cli::parser::TokenType> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const downward::cli::parser::TokenType& t, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", token_type_name(t));
    }
};

template <>
struct formatter<downward::cli::parser::Token> {
    bool text = false;

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        auto it = std::ranges::begin(ctx);
        const auto end = std::ranges::end(ctx);

        for (; it != end && *it != '}'; ++it) {
            switch (*it) {
            case 't':
                text = true;
                break;
            default:
                throw std::format_error("Could not parse specifier!");
            }
        }

        return it;
    }

    template <typename FormatContext>
    auto
    format(const downward::cli::parser::Token& token, FormatContext& ctx) const
    {
        return std::format_to(
            ctx.out(),
            "<Type: '{}', Value: '{}'>",
            token.type,
            token.content);
    }
};

} // namespace std
#endif
