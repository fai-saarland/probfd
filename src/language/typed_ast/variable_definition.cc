#include "language/typed_ast/variable_definition.h"

#include "language/typed_ast/decorated_variable_node.h"

#include <utility>

using namespace std;

namespace downward::cli::parser {

VariableDeclaration::VariableDeclaration(std::string variable_name)
    : variable_name(std::move(variable_name))
{
}

VariableDeclaration::VariableDeclaration(VariableDeclaration&& other) noexcept
    : variable_name(std::move(other.variable_name))
    , usages(std::move(other.usages))
{
    for (VariableNode* u : usages) { u->declaration = this; }
}

VariableDeclaration&
VariableDeclaration::operator=(VariableDeclaration&& other) noexcept
{
    variable_name = std::move(other.variable_name);
    usages = std::move(other.usages);

    for (VariableNode* u : usages) { u->declaration = this; }

    return *this;
}

VariableDefinition::VariableDefinition(
    std::string variable_name,
    DecoratedASTNodePtr variable_expression)
    : VariableDeclaration(std::move(variable_name))
    , variable_expression(std::move(variable_expression))
{
}

VariableDefinition::VariableDefinition(VariableDefinition&& other) noexcept =
    default;

VariableDefinition::~VariableDefinition() = default;

VariableDefinition&
VariableDefinition::operator=(VariableDefinition&& other) noexcept = default;

} // namespace downward::cli::parser