#ifndef LANGUAGE_TOKEN_STREAM_H
#define LANGUAGE_TOKEN_STREAM_H

#include <format>
#include <string>
#include <vector>

namespace language {
class Context;
}

namespace language::parser {
enum class TokenType;
struct Token;
}

namespace language::parser {

class TokenStream {
    std::vector<Token> tokens;
    std::size_t pos;

public:
    explicit TokenStream(std::vector<Token>&& tokens);

    TokenStream(const TokenStream& other);
    TokenStream(TokenStream&& other) noexcept;

    TokenStream& operator=(const TokenStream& other);
    TokenStream& operator=(TokenStream&& other) noexcept;

    ~TokenStream();

    bool has_tokens(std::size_t n) const;
    Token peek(const Context& context, std::size_t n = 0) const;
    Token pop(const Context& context);
    Token pop(const Context& context, TokenType expected_type);

    std::size_t get_position() const;
    std::size_t size() const;
    std::string str(std::size_t from, std::size_t to) const;
};

} // namespace language::parser

#endif
