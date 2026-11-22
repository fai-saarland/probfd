#ifndef LANGUAGE_PARSER_DECLARATION_H
#define LANGUAGE_PARSER_DECLARATION_H

#include <memory>
#include <string>
#include <vector>

namespace language::parser {
class DecoratedExpression;
class DecoratedLocalIdentifierExpression;
} // namespace language::parser

namespace language::parser {

struct Declaration {
    std::string identifier;

    explicit Declaration(std::string variable_name);

    Declaration(Declaration&& other) noexcept = default;
    Declaration& operator=(Declaration&& other) noexcept = default;
};

struct VariableDefinition : public Declaration {
    std::unique_ptr<DecoratedExpression> variable_expression;
    std::vector<DecoratedLocalIdentifierExpression*> usages;
    mutable size_t stack_index = std::numeric_limits<std::size_t>::max();

    VariableDefinition(
        std::string variable_name,
        std::unique_ptr<DecoratedExpression> variable_expression);

    VariableDefinition(VariableDefinition&& other) noexcept;
    VariableDefinition& operator=(VariableDefinition&& other) noexcept;
};

} // namespace language::parser
#endif
