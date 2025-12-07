#ifndef LANGUAGE_LEXICAL_ANALYZER_H
#define LANGUAGE_LEXICAL_ANALYZER_H

#include <string>

namespace downward::cli::parser {
class TokenStream;
}

namespace downward::cli::parser {
extern TokenStream split_tokens(const std::string& text);
} // namespace downward::cli::parser
#endif
