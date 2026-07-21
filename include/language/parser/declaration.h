#ifndef LANGUAGE_PARSER_DECLARATION_H
#define LANGUAGE_PARSER_DECLARATION_H

#include <memory>
#include <string>
#include <vector>

namespace language::parser {
class DecoratedExpression;
class DecoratedIdentifierExpression;
}

namespace language::parser {

struct Declaration {
    std::string identifier;
    std::vector<DecoratedIdentifierExpression*> usages;

    explicit Declaration(std::string variable_name);

    Declaration(Declaration&& other) noexcept;
    Declaration& operator=(Declaration&& other) noexcept;
};

struct VariableDefinition : public Declaration {
    std::unique_ptr<DecoratedExpression> variable_expression;

    VariableDefinition(
        std::string variable_name,
        std::unique_ptr<DecoratedExpression> variable_expression);

    VariableDefinition(VariableDefinition&& other) noexcept = default;

    VariableDefinition&
    operator=(VariableDefinition&& other) noexcept = default;
};

} // namespace language::parser
#endif
