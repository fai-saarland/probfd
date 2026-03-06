#include "language/ast/direct_function_call_node.h"

#include "language/typed_ast/convert_node.h"
#include "language/typed_ast/decorated_feature_literal_node.h"
#include "language/typed_ast/decorated_function_call_node.h"
#include "language/typed_ast/variable_environment.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "language/typed_ast/types.h"

#include "language/context.h"
#include "language/syntax_analyzer.h"

#include <unordered_map>
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

DirectFunctionCallNode::DirectFunctionCallNode(
    QualifiedName callee,
    vector<std::unique_ptr<ExpressionNode>>&& positional_arguments,
    unordered_map<string, std::unique_ptr<ExpressionNode>>&& keyword_arguments)
    : callee(std::move(callee))
    , positional_arguments(move(positional_arguments))
    , keyword_arguments(move(keyword_arguments))
{
}

TypedDecoratedAstNodePtr DirectFunctionCallNode::static_analysis(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry& type_registry) const
{
    TraceBlock block(context, "Checking Call");

    TraceBlock nblock(context, "Checking Callee Identifier: {}", callee);

    auto decls_vnt = env.get_variable_declaration(local_env, callee, context);

    return std::visit(
        utils::overload{
            [&, this](typed_ast::TypedValue tdecl) {
                return this->construct_indirect_call(
                    context,
                    env,
                    local_env,
                    type_registry,
                    tdecl);
            },
            [&,
             this](const std::vector<typed_ast::TypedFunctionValue>*& tdecl) {
                return this->construct_call(
                    context,
                    env,
                    local_env,
                    type_registry,
                    *tdecl);
            },
        },
        decls_vnt);
}

TypedDecoratedAstNodePtr DirectFunctionCallNode::construct_indirect_call(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry& type_registry,
    const typed_ast::TypedValue& tdecl) const
{
    const auto& [type, decl] = tdecl;

    if (!type->is_function_type()) {
        context.error("Variable '{}' does not have function type", callee);
    }

    const auto* callee_type = static_cast<const typed_ast::FunctionType*>(type);

    if (!keyword_arguments.empty()) {
        context.error("Keyword arguments not allowed here");
    }

    const auto& argument_types = callee_type->get_argument_types();

    if (positional_arguments.size() != argument_types.size()) {
        context.error(
            "Expected {} arguments, {} were given",
            argument_types.size(),
            positional_arguments.size());
    }

    vector<typed_ast::FunctionArgument> arguments;
    arguments.reserve(argument_types.size());

    for (std::size_t i = 0; i < positional_arguments.size(); ++i) {
        TraceBlock nnblock(
            context,
            "Checking the positional argument at index {}",
            i);

        const auto& arg = positional_arguments[i];
        const auto& arg_type = argument_types[i];

        std::unique_ptr<typed_ast::DecoratedExpressionNode> decorated_arg =
            decorate_and_convert(
                *arg,
                *arg_type,
                context,
                env,
                local_env,
                type_registry);

        arguments.emplace_back(move(decorated_arg), false);
    }

    std::unique_ptr<typed_ast::DecoratedExpressionNode> callee_node =
        decl->create_load_node();

    return {
        std::make_unique<typed_ast::DecoratedFunctionCallNode>(
            std::move(callee_node),
            move(arguments)),
        &callee_type->get_return_type()};
}

TypedDecoratedAstNodePtr DirectFunctionCallNode::construct_call(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry& type_registry,
    const std::vector<typed_ast::TypedFunctionValue>& function_declarations)
    const
{
    // Callee symbol denotes one or multiple function overloads
    if (function_declarations.size() > 1) {
        context.error(
            "Symbol '{}' refers to multiple function overloads."
            "Overload resolution is currently not supported.",
            callee);
    }

    const auto& [ftype, fdecl] = function_declarations.front();
    const auto& argument_types = ftype->get_argument_types();

    const std::vector<ArgumentInfo>& argument_infos = fdecl->get_arguments();

    // Check keyword arguments exist
    for (const auto& keyword : keyword_arguments | std::views::keys) {
        for (const auto& arg_info : argument_infos) {
            if (arg_info.key == keyword) { goto next; }
        }

        context.error("Unknown keyword argument {}", keyword);

    next:;
    }

    vector<typed_ast::FunctionArgument> arguments;
    arguments.reserve(argument_types.size());

    for (std::size_t i = 0; i < positional_arguments.size(); ++i) {
        TraceBlock nnblock(
            context,
            "Checking the positional argument at index {}",
            i);

        const auto& arg_info = argument_infos[i];
        const auto& arg_type = argument_types[i];
        const auto& pos_arg = *positional_arguments[i];

        if (keyword_arguments.contains(arg_info.key)) {
            context.error(
                "The argument '{}' is defined by the positional "
                "argument "
                "at index {} and by a keyword argument.",
                arg_info.key,
                i);
        }

        std::unique_ptr<typed_ast::DecoratedExpressionNode> decorated_arg =
            decorate_and_convert(
                pos_arg,
                *arg_type,
                context,
                env,
                local_env,
                type_registry);

        arguments.emplace_back(move(decorated_arg), false);
    }

    for (std::size_t i = positional_arguments.size(); i < argument_infos.size();
         ++i) {
        TraceBlock nnblock(
            context,
            "Checking the positional argument at index {}",
            i);
        const auto& arg_info = argument_infos[i];
        const auto& arg_type = argument_types[i];

        if (auto it = keyword_arguments.find(arg_info.key);
            it != keyword_arguments.end()) {
            const auto& keyword_arg = it->second;

            std::unique_ptr<typed_ast::DecoratedExpressionNode> decorated_arg =
                decorate_and_convert(
                    *keyword_arg,
                    *arg_type,
                    context,
                    env,
                    local_env,
                    type_registry);

            arguments.emplace_back(move(decorated_arg), false);
        } else if (arg_info.has_default()) {
            std::unique_ptr<ExpressionNode> default_arg = [&] {
                TraceBlock nnnblock(context, "Parsing default value");
                return tokenize_and_parse(arg_info.default_value);
            }();

            std::unique_ptr<typed_ast::DecoratedExpressionNode> decorated_arg =
                decorate_and_convert(
                    *default_arg,
                    *arg_type,
                    context,
                    env,
                    local_env,
                    type_registry);

            arguments.emplace_back(move(decorated_arg), true);
        } else {
            context.error("Missing argument '{}' is mandatory.", arg_info.key);
        }
    }

    std::unique_ptr<typed_ast::DecoratedExpressionNode> callee_node =
        fdecl->create_load_node();

    return {
        std::make_unique<typed_ast::DecoratedFunctionCallNode>(
            std::move(callee_node),
            move(arguments)),
        &ftype->get_return_type()};
}

} // namespace language::parser
