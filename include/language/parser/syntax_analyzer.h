#ifndef LANGUAGE_PARSER_SYNTAX_ANALYZER_H
#define LANGUAGE_PARSER_SYNTAX_ANALYZER_H

#include "language/parser/abstract_syntax_tree.h"

namespace language::parser {
class TokenStream;

extern ASTNodePtr parse(TokenStream& tokens);
extern ASTNodePtr tokenize_and_parse(const std::string& expression);

} // namespace language::parser
#endif
