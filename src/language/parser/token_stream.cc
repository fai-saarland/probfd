#include "language/parser/token_stream.h"

#include "language/context.h"

#include <cassert>
#include <sstream>

using namespace std;

namespace language::parser {

Token::Token(string content, TokenType type)
    : content(std::move(content))
    , type(type)
{
}

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

string token_type_name(TokenType token_type)
{
    switch (token_type) {
    case TokenType::OPENING_PARENTHESIS: return "(";
    case TokenType::CLOSING_PARENTHESIS: return ")";
    case TokenType::OPENING_BRACKET: return "[";
    case TokenType::CLOSING_BRACKET: return "]";
    case TokenType::COMMA: return ",";
    case TokenType::EQUALS: return "=";
    case TokenType::LET: return "Let";
    case TokenType::AS: return "As";
    case TokenType::IN: return "In";
    case TokenType::TRUE: return "True";
    case TokenType::FALSE: return "False";
    case TokenType::STRING: return "String";
    case TokenType::INTEGER: return "Integer";
    case TokenType::FLOAT: return "Float";
    case TokenType::IDENTIFIER: return "Identifier";
    default: abort();
    }
}

ostream& operator<<(ostream& out, TokenType token_type)
{
    out << token_type_name(token_type);
    return out;
}

ostream& operator<<(ostream& out, const Token& token)
{
    out << "<Type: '" << token.type << "', Value: '" << token.content << "'>";
    return out;
}

} // namespace language::parser
