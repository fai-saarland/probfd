#ifndef LANGUAGE_SYNTAX_ANALYZER_H
#define LANGUAGE_SYNTAX_ANALYZER_H

#include <memory>
#include <string>

namespace downward::cli::parser {
class TokenStream;
class ASTNode;
} // namespace downward::cli::parser

namespace downward::cli::parser {

extern std::unique_ptr<ASTNode> parse(TokenStream& tokens);

extern std::unique_ptr<ASTNode>
tokenize_and_parse(const std::string& expression);

} // namespace downward::cli::parser
#endif
