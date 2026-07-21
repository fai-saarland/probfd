#include "language/parser/declaration.h"

#include "language/parser/decorated_abstract_syntax_tree.h"

#include <functional>

using namespace std;

namespace language::parser {

Declaration::Declaration(std::string variable_name)
    : identifier(std::move(variable_name))
{
}

Declaration::Declaration(Declaration&& other) noexcept
    : identifier(std::move(other.identifier))
    , usages(std::move(other.usages))
{
    for (DecoratedIdentifierExpression* u : usages) {
        u->declaration = this;
    }
}

Declaration& Declaration::operator=(Declaration&& other) noexcept
{
    identifier = std::move(other.identifier);
    usages = std::move(other.usages);

    for (DecoratedIdentifierExpression* u : usages) {
        u->declaration = this;
    }

    return *this;
}

VariableDefinition::VariableDefinition(
    std::string variable_name,
    std::unique_ptr<DecoratedExpression> variable_expression)
    : Declaration(std::move(variable_name))
    , variable_expression(std::move(variable_expression))
{
}

} // namespace language::parser