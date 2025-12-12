#ifndef LANGUAGE_LEXICAL_ANALYZER_H
#define LANGUAGE_LEXICAL_ANALYZER_H

#include <string>

namespace language::parser {
class TokenStream;
}

namespace language::parser {
extern TokenStream split_tokens(const std::string& text);
} // namespace downward::cli::parser
#endif
