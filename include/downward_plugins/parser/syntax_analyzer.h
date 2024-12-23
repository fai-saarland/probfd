#ifndef PARSER_SYNTAX_ANALYZER_H
#define PARSER_SYNTAX_ANALYZER_H

#include "downward_plugins/parser/abstract_syntax_tree.h"

namespace downward_plugins::parser {
class TokenStream;

extern ASTNodePtr parse(TokenStream& tokens);
} // namespace downward_plugins::parser
#endif
