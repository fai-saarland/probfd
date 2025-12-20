#include "language/token.h"

#include "language/critical_error.h"

#include <algorithm>
#include <iostream>

#undef IN
#undef TRUE
#undef FALSE

using namespace std;

namespace {

std::string to_lower(const string& content)
{
    std::string lower;
    lower.reserve(content.size());
    ranges::transform(content, lower.begin(), [](char c) {
        return std::tolower(c);
    });
    return lower;
}

} // namespace

namespace language::parser {
static string case_insensitive_to_lower(const string& content, TokenType type)
{
    if (type == TokenType::TRUE || type == TokenType::FALSE ||
        type == TokenType::INTEGER || type == TokenType::FLOAT ||
        type == TokenType::IDENTIFIER) {
        return to_lower(content);
    } else {
        return content;
    }
}

Token::Token(const string& content, TokenType type)
    : content(case_insensitive_to_lower(content, type))
    , type(type)
{
}

string token_type_name(TokenType token_type)
{
    switch (token_type) {
    case TokenType::OPENING_PARENTHESIS: return "(";
    case TokenType::CLOSING_PARENTHESIS: return ")";
    case TokenType::OPENING_BRACKET: return "[";
    case TokenType::CLOSING_BRACKET: return "]";
    case TokenType::COMMA: return ",";
    case TokenType::DOT: return ".";
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
    default: throw CriticalError("Unknown token type.");
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
