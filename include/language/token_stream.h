#ifndef LANGUAGE_TOKEN_STREAM_H
#define LANGUAGE_TOKEN_STREAM_H

#include <format>
#include <string>
#include <vector>

namespace downward::utils {
class Context;
}

namespace downward::cli::parser {
enum class TokenType;
struct Token;
}

namespace downward::cli::parser {

class TokenStream {
    std::vector<Token> tokens;
    int pos;

public:
    explicit TokenStream(std::vector<Token>&& tokens);

    TokenStream(const TokenStream& other);
    TokenStream(TokenStream&& other) noexcept;

    TokenStream& operator=(const TokenStream& other);
    TokenStream& operator=(TokenStream&& other) noexcept;

    ~TokenStream();

    bool has_tokens(int n) const;
    Token peek(const utils::Context& context, int n = 0) const;
    Token pop(const utils::Context& context);
    Token pop(const utils::Context& context, TokenType expected_type);

    int get_position() const;
    int size() const;
    std::string str(int from, int to) const;
};

} // namespace downward::cli::parser

#endif
