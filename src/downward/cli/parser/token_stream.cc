#include "downward/cli/parser/token_stream.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/strings.h"
#include "downward/utils/system.h"

#include <cassert>
#include <sstream>

#undef IN

using namespace std;

namespace downward::cli::parser {
static string case_insensitive_to_lower(const string& content, TokenType type)
{
    if (type == TokenType::TRUE || type == TokenType::FALSE ||
        type == TokenType::INTEGER || type == TokenType::FLOAT ||
        type == TokenType::IDENTIFIER) {
        return utils::tolower(content);
    } else {
        return content;
    }
}

Token::Token(const string& content, TokenType type)
    : content(case_insensitive_to_lower(content, type))
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
    return std::format("{:n:t}", subrange);
}

string token_type_name(TokenType token_type)
{
    switch (token_type) {
    case TokenType::OPENING_PARENTHESIS: return "(";
    case TokenType::CLOSING_PARENTHESIS: return ")";
    case TokenType::OPENING_BRACKET: return "[";
    case TokenType::CLOSING_BRACKET: return "]";
    case TokenType::COMMA: return ",";
    case TokenType::COLON: return ":";
    case TokenType::EQUALS: return "=";
    case TokenType::PLUS: return "+";
    case TokenType::MINUS: return "-";
    case TokenType::LET: return "Let";
    case TokenType::AS: return "As";
    case TokenType::IN: return "In";
    case TokenType::LAMBDA: return "Lambda";
    case TokenType::TRUE: return "true";
    case TokenType::FALSE: return "false";
    case TokenType::STRING: return "String";
    case TokenType::INTEGER: return "Integer";
    case TokenType::FLOAT: return "Float";
    case TokenType::IDENTIFIER: return "Identifier";
    case TokenType::TYPE_BOOL: return "bool";
    case TokenType::TYPE_STRING: return "string";
    case TokenType::TYPE_INTEGER: return "int";
    case TokenType::TYPE_FLOAT: return "float";
    default: throw utils::CriticalError("Unknown token type.");
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
} // namespace downward::cli::parser
