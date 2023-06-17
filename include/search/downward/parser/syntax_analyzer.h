#ifndef PARSER_SYNTAX_ANALYZER_H
#define PARSER_SYNTAX_ANALYZER_H

#include "downward/parser/abstract_syntax_tree.h"

namespace parser {
class TokenStream;

extern ASTNodePtr parse(TokenStream& tokens);
} // namespace parser
#endif
