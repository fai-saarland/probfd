#ifndef LANGUAGE_SYNTAX_ANALYZER_H
#define LANGUAGE_SYNTAX_ANALYZER_H

#include <memory>
#include <string>

namespace language::parser {
class TokenStream;
class ASTNode;
} // namespace downward::cli::parser

namespace language::parser {

extern std::unique_ptr<ASTNode> parse(TokenStream& tokens);

extern std::unique_ptr<ASTNode>
tokenize_and_parse(const std::string& expression);

} // namespace downward::cli::parser
#endif
