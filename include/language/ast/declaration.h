#ifndef LANGUAGE_AST_DECLARATION_H
#define LANGUAGE_AST_DECLARATION_H

#include <string>

namespace language::parser {

// All syntactic constructs that introduce a named entity.
class Declaration {
protected:
    std::string identifier;

public:
    explicit Declaration(std::string identifier);

    Declaration(Declaration&& other) noexcept = default;

    Declaration& operator=(Declaration&& other) noexcept = default;

    const std::string& get_identifier() const { return identifier; }
};

} // namespace language::parser

#endif
