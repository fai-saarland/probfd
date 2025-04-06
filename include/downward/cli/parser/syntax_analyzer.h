#ifndef PARSER_SYNTAX_ANALYZER_H
#define PARSER_SYNTAX_ANALYZER_H

#include "downward/cli/parser/abstract_syntax_tree.h"

namespace downward::cli::parser {
class TokenStream;

extern ASTNodePtr parse(TokenStream& tokens);
} // namespace downward::cli::parser
#endif
