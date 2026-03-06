#ifndef LANGUAGE_SYNTAX_ANALYZER_H
#define LANGUAGE_SYNTAX_ANALYZER_H

#include <memory>
#include <string>

namespace language::parser {
class TokenStream;
class ExpressionNode;
} // namespace language::parser

namespace language::parser {

extern std::unique_ptr<ExpressionNode> parse(TokenStream& tokens);

extern std::unique_ptr<ExpressionNode>
tokenize_and_parse(const std::string& expression);

} // namespace language::parser
#endif
