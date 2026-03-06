#include "language/typed_ast/decorated_let_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/typed_ast/decorated_variable_node.h"
#include "language/typed_ast/local_value_definition.h"

#include <algorithm>
#include <functional>
#include <ranges>

using namespace std;

namespace language::typed_ast {

DecoratedLetNode::DecoratedLetNode(
    std::vector<std::unique_ptr<LocalValueDefinition>>
        decorated_variable_definitions,
    std::unique_ptr<DecoratedExpressionNode> nested_value)
    : decorated_variable_definitions(move(decorated_variable_definitions))
    , nested_value(move(nested_value))
{
}

DecoratedLetNode::~DecoratedLetNode() = default;

DecoratedLetNode::DecoratedLetNode(DecoratedLetNode&&) noexcept = default;

DecoratedLetNode&
DecoratedLetNode::operator=(DecoratedLetNode&&) noexcept = default;

std::any DecoratedLetNode::construct(ConstructContext& context) const
{
    TraceBlock lblock(context, "Constructing let-expression");

    for (std::size_t index = 0;
         const auto& def : decorated_variable_definitions) {
        TraceBlock block(context, "Pushing let variable #{}", index++);
        context.push_variable(def->variable_expression->construct(context));
    }

    std::any result = [&] {
        TraceBlock block(context, "Constructing nested value");
        return nested_value->construct(context);
    }();

    context.pop_variables(decorated_variable_definitions.size());

    return result;
}

} // namespace language::typed_ast