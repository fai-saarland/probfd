#include "language/typed_ast/decorated_variable_node.h"

#include "language/typed_ast/construct_context.h"
#include "language/typed_ast/variable_environment.h"

#include "language/context.h"

#include <print>

using namespace std;

namespace language::typed_ast {

VariableNode::VariableNode(StackValue& value)
    : value(&value)
{
    value.add_user(this);
}

std::any VariableNode::construct(ConstructContext& context) const
{
    const std::size_t stack_index = value->get_index();

    TraceBlock block(
        context,
        "Looking up variable at stack index '{}'",
        stack_index);

    return context.get_variable(stack_index);
}

} // namespace language::typed_ast