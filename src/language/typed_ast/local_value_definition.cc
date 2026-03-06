#include "language/typed_ast/local_value_definition.h"

#include "language/typed_ast/decorated_variable_node.h"

#include <algorithm>

using namespace std;

namespace language::typed_ast {

LocalValueDefinition::LocalValueDefinition(
    std::size_t index,
    std::unique_ptr<DecoratedExpressionNode> variable_expression)
    : StackValue(index)
    , variable_expression(std::move(variable_expression))
{
}

LocalValueDefinition::LocalValueDefinition(
    LocalValueDefinition&& other) noexcept = default;

LocalValueDefinition::~LocalValueDefinition() = default;

LocalValueDefinition& LocalValueDefinition::operator=(
    LocalValueDefinition&& other) noexcept = default;

std::unique_ptr<DecoratedExpressionNode>
LocalValueDefinition::create_load_node()
{
    return std::make_unique<VariableNode>(*this);
}

} // namespace language::typed_ast