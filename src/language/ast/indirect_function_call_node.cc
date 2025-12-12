#include "language/ast/indirect_function_call_node.h"

#include "language/typed_ast/convert_node.h"
#include "language/typed_ast/decorated_function_call_node.h"

#include "language/plugins/plugin.h"
#include "language/plugins/types.h"

#include "language/context.h"

#include <vector>

using namespace std;

namespace language::parser {

static DecoratedASTNodePtr decorate_and_convert(
    const ASTNode& node,
    const plugins::Type& target_type,
    Context& context,
    VariableEnvironment& env)
{
    auto [ast_node, type] = node.static_analysis(context, env);

    if (*type != target_type) {
        TraceBlock block(context, "Adding casting node");
        if (type->can_convert_into(target_type)) {
            return std::make_unique<ConvertNode>(
                move(ast_node),
                *type,
                target_type);
        }

        context.error(
            "Cannot convert from type '{}' to type '{}'\n",
            type->name(),
            target_type.name());
    }
    return std::move(ast_node);
}

IndirectFunctionCallNode::IndirectFunctionCallNode(
    ASTNodePtr callee,
    vector<ASTNodePtr>&& positional_arguments,
    const string& unparsed_config)
    : callee(std::move(callee))
    , positional_arguments(move(positional_arguments))
    , unparsed_config(unparsed_config)
{
}

TypedDecoratedAstNodePtr IndirectFunctionCallNode::static_analysis(
    Context& context,
    VariableEnvironment& env) const
{
    TraceBlock block(context, "Checking Call");

    auto [dast_node, type] = callee->static_analysis(context, env);
    if (!type->is_function_type()) {
        context.error("Callee does not have function type.");
    }

    const auto& ftype = *static_cast<const plugins::FunctionType*>(type);
    const auto& argument_types = ftype.get_argument_types();

    vector<FunctionArgument> arguments;
    arguments.reserve(argument_types.size());

    if (positional_arguments.size() != argument_types.size()) {
        context.error(
            "Expected {} arguments, {} were given",
            argument_types.size(),
            positional_arguments.size());
    }

    for (std::size_t i = 0; i < positional_arguments.size(); ++i) {
        TraceBlock nblock(
            context,
            "Checking the positional argument at index {}",
            i);

        const auto& arg = positional_arguments[i];
        const auto& arg_type = argument_types[i];

        DecoratedASTNodePtr decorated_arg =
            decorate_and_convert(*arg, *arg_type, context, env);

        arguments.emplace_back(move(decorated_arg), false);
    }

    return {
        std::make_unique<DecoratedFunctionCallNode>(
            std::move(dast_node),
            move(arguments),
            unparsed_config),
        &ftype.get_return_type()};
}

} // namespace language::parser
