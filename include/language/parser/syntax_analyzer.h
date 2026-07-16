#ifndef LANGUAGE_PARSER_SYNTAX_ANALYZER_H
#define LANGUAGE_PARSER_SYNTAX_ANALYZER_H

#include <memory>
#include <string>

namespace language::parser {

class Expression;
class TokenStream;

extern std::unique_ptr<Expression> parse(TokenStream& tokens);

extern std::unique_ptr<Expression>
tokenize_and_parse(const std::string& expression);

} // namespace language::parser
#endif
