#include "language/token_stream.h"

#include "language/context.h"
#include "language/token.h"

#include <cassert>
#include <ranges>

using namespace std;

namespace language::parser {

TokenStream::TokenStream(vector<Token>&& tokens)
    : tokens(move(tokens))
    , pos(0)
{
}

TokenStream::TokenStream(const TokenStream& other) = default;
TokenStream::TokenStream(TokenStream&& other) noexcept = default;

TokenStream& TokenStream::operator=(const TokenStream& other) = default;
TokenStream& TokenStream::operator=(TokenStream&& other) noexcept = default;

TokenStream::~TokenStream() = default;

bool TokenStream::has_tokens(std::size_t n) const
{
    assert(n > 0);
    return pos - 1 + n < tokens.size();
}

Token TokenStream::peek(const Context& context, std::size_t n) const
{
    if (!has_tokens(n + 1)) {
        context.error(
            "{} token(s) required, but {} remain.",
            n + 1,
            tokens.size() - pos);
    }
    return tokens[pos + n];
}

Token TokenStream::pop(const Context& context)
{
    if (!has_tokens(1)) { context.error("Input stream exhausted."); }
    return tokens[pos++];
}

Token TokenStream::pop(const Context& context, TokenType expected_type)
{
    if (!has_tokens(1)) {
        context.error(
            "Input stream exhausted while expecting token of type '{}'.",
            expected_type);
    }

    Token token = pop(context);
    if (token.type != expected_type) {
        context.error(
            "Got token {}. Expected token of type '{}'.",
            token,
            expected_type);
    }
    return token;
}

std::size_t TokenStream::get_position() const
{
    return pos;
}

std::size_t TokenStream::size() const
{
    return tokens.size();
}

string TokenStream::str(std::size_t from, std::size_t to) const
{
    const auto b = std::ranges::begin(tokens);
    std::ranges::subrange subrange(
        std::next(b, from),
        std::next(b, std::min(to, tokens.size())));
    return std::format(
        "{:n:s}",
        subrange | std::views::transform(&Token::content));
}

} // namespace language::parser
