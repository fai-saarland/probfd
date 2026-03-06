#include "language/ast/indirect_function_call_node.h"

#include "language/typed_ast/convert_node.h"
#include "language/typed_ast/decorated_function_call_node.h"

#include "language/ast/internal_function_definition.h"

#include "language/typed_ast/types.h"

#include "language/context.h"

#include <vector>

using namespace std;

namespace language::parser {

static std::unique_ptr<typed_ast::DecoratedExpressionNode> decorate_and_convert(
    const ExpressionNode& node,
    const typed_ast::Type& target_type,
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry& type_registry)
{
    auto [ast_node, type] =
        node.static_analysis(context, env, local_env, type_registry);

    if (*type != target_type) {
        TraceBlock block(context, "Adding casting node");
        if (type->can_convert_into(target_type)) {
            return std::make_unique<typed_ast::ConvertNode>(
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
    std::unique_ptr<ExpressionNode> callee,
    vector<std::unique_ptr<ExpressionNode>>&& positional_arguments)
    : callee(std::move(callee))
    , positional_arguments(move(positional_arguments))
{
}

TypedDecoratedAstNodePtr IndirectFunctionCallNode::static_analysis(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry& type_registry) const
{
    TraceBlock block(context, "Checking Call");

    auto [dast_node, type] =
        callee->static_analysis(context, env, local_env, type_registry);

    if (!type->is_function_type()) {
        context.error("Callee does not have function type.");
    }

    const auto& ftype = *static_cast<const typed_ast::FunctionType*>(type);
    const auto& argument_types = ftype.get_argument_types();

    vector<typed_ast::FunctionArgument> arguments;
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

        auto decorated_arg = decorate_and_convert(
            *arg,
            *arg_type,
            context,
            env,
            local_env,
            type_registry);

        arguments.emplace_back(move(decorated_arg), false);
    }

    return {
        std::make_unique<typed_ast::DecoratedFunctionCallNode>(
            std::move(dast_node),
            move(arguments)),
        &ftype.get_return_type()};
}

} // namespace language::parser
