#ifndef LANGUAGE_PARSER_TOKEN_STREAM_H
#define LANGUAGE_PARSER_TOKEN_STREAM_H

#include <string>
#include <vector>

namespace language {
class Context;
}

namespace language::parser {

enum class TokenType;
struct Token;

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

} // namespace language::parser

#endif
