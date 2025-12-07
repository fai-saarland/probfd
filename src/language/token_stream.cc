#include "language/token_stream.h"

#include "language/token.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#include <cassert>

using namespace std;

namespace downward::cli::parser {

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

bool TokenStream::has_tokens(int n) const
{
    assert(n > 0);
    return utils::in_bounds(pos + n - 1, tokens);
}

Token TokenStream::peek(const utils::Context& context, int n) const
{
    if (!has_tokens(n + 1)) {
        context.error(
            "{} token(s) required, but {} remain.",
            n + 1,
            tokens.size() - pos);
    }
    return tokens[pos + n];
}

Token TokenStream::pop(const utils::Context& context)
{
    if (!has_tokens(1)) { context.error("Input stream exhausted."); }
    return tokens[pos++];
}

Token TokenStream::pop(const utils::Context& context, TokenType expected_type)
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

int TokenStream::get_position() const
{
    return pos;
}

int TokenStream::size() const
{
    return tokens.size();
}

string TokenStream::str(int from, int to) const
{
    const auto b = std::ranges::begin(tokens);
    std::ranges::subrange subrange(
        std::next(b, from),
        std::next(b, std::min<int>(to, tokens.size())));
    return std::format(
        "{:n:s}",
        subrange | std::views::transform(&Token::content));
}

} // namespace downward::cli::parser
