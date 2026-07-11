#include "language/parser/token_stream.h"

#include "language/parser/token.h"

#include "language/context.h"

#include <cassert>
#include <sstream>

using namespace std;

namespace language::parser {

TokenStream::TokenStream(vector<Token>&& tokens)
    : tokens(move(tokens))
    , pos(0)
{
}

bool TokenStream::has_tokens(int n) const
{
    assert(n > 0);
    const int index = pos + n - 1;
    return index >= 0 && static_cast<size_t>(index) < tokens.size();
}

Token TokenStream::peek(const Context& context, int n) const
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
    int curr_position = max(0, from);
    int max_position = min(static_cast<int>(tokens.size()), to);
    ostringstream message;
    while (curr_position < max_position) {
        message << tokens[curr_position].content;
        curr_position++;
    }
    return message.str();
}

} // namespace language::parser
