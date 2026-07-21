#ifndef LANGUAGE_PARSER_LEXICAL_ANALYZER_H
#define LANGUAGE_PARSER_LEXICAL_ANALYZER_H

#include <string>

namespace language::parser {
class TokenStream;

extern TokenStream split_tokens(std::string_view text);
} // namespace language::parser
#endif
