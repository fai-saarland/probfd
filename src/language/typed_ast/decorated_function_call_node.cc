#include "language/typed_ast/decorated_function_call_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/typed_ast/type_aliases.h"

#include <any>
#include <ranges>

using namespace std;

namespace language::typed_ast {

FunctionArgument::FunctionArgument(
    std::unique_ptr<DecoratedExpressionNode> value,
    bool is_default)
    : value(move(value))
    , is_default(is_default)
{
}

DecoratedExpressionNode& FunctionArgument::get_value()
{
    return *value;
}

const DecoratedExpressionNode& FunctionArgument::get_value() const
{
    return *value;
}

bool FunctionArgument::is_default_argument() const
{
    return is_default;
}

DecoratedFunctionCallNode::DecoratedFunctionCallNode(
    std::unique_ptr<DecoratedExpressionNode> callee,
    vector<FunctionArgument> arguments)
    : callee(std::move(callee))
    , arguments(std::move(arguments))
{
}

std::any DecoratedFunctionCallNode::construct(ConstructContext& context) const
{
    TraceBlock cblock(context, "Constructing callee");

    const auto calleef =
        std::any_cast<AnyFunctionType>(callee->construct(context));

    for (std::size_t i = 0; const auto& arg : arguments) {
        TraceBlock block(context, "Constructing argument {}", i);
        context.push_variable(arg.get_value().construct(context));
    }

    return calleef(context);
}

} // namespace language::parser