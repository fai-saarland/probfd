#include "language/parser/token.h"

#include <ostream>
#include <utility>

using namespace std;

namespace language::parser {

string token_type_name(TokenType token_type)
{
    switch (token_type) {
    case TokenType::OPENING_PARENTHESIS: return "(";
    case TokenType::CLOSING_PARENTHESIS: return ")";
    case TokenType::OPENING_BRACKET: return "[";
    case TokenType::CLOSING_BRACKET: return "]";
    case TokenType::COMMA: return ",";
    case TokenType::EQUALS: return "=";
    case TokenType::PLUS: return "+";
    case TokenType::MINUS: return "-";
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
