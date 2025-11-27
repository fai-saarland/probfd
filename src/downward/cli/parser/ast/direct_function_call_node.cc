#include "downward/cli/parser/ast/direct_function_call_node.h"

#include "downward/cli/parser/ast/variable_environment.h"
#include "downward/cli/parser/decorated_abstract_syntax_tree.h"
#include "downward/cli/parser/syntax_analyzer.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"
#include "downward/cli/plugins/types.h"

#include "downward/utils/logging.h"

#include <unordered_map>
#include <vector>

using namespace std;

namespace downward::cli::parser {


static DecoratedASTNodePtr decorate_and_convert(
    const ASTNode& node,
    const plugins::Type& target_type,
    utils::Context& context,
    VariableEnvironment& env)
{
    auto [ast_node, type] = node.static_analysis(context, env);

    if (*type != target_type) {
        utils::TraceBlock block(context, "Adding casting node");
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


DirectFunctionCallNode::DirectFunctionCallNode(
    QualifiedName callee,
    vector<ASTNodePtr>&& positional_arguments,
    unordered_map<string, ASTNodePtr>&& keyword_arguments,
    const string& unparsed_config)
    : callee(std::move(callee))
    , positional_arguments(move(positional_arguments))
    , keyword_arguments(move(keyword_arguments))
    , unparsed_config(unparsed_config)
{
}

TypedDecoratedAstNodePtr DirectFunctionCallNode::static_analysis(
    utils::Context& context,
    VariableEnvironment& env) const
{
    utils::TraceBlock block(context, "Checking Call");

    utils::TraceBlock nblock(context, "Checking Callee Identifier: {}", callee);

    const auto& [qualification, name] = callee;

    DecoratedASTNodePtr callee_node;
    const plugins::FunctionType* callee_type;
    std::vector<plugins::ArgumentInfo> argument_infos;

    if (qualification.empty() && env.has_variable(name)) {
        auto& def = env.get_variable_definition(name);
        callee_node = std::make_unique<VariableNode>(def);
        def.usages.emplace_back(static_cast<VariableNode*>(callee_node.get()));

        const auto& type = &env.get_variable_type(name);

        if (!type->is_function_type()) {
            context.error(
                "Variable '{}' has type '{}' which is not a function type",
                callee,
                type->name());
        }

        callee_type = static_cast<const plugins::FunctionType*>(type);
    } else if (const auto& n = env.get_registry().get_namespace(qualification);
               n.has_function(name)) {
        const auto& f = n.get_function_definition(name);
        callee_node = std::make_unique<FeatureLiteralNode>(f);
        callee_type = &f.get_type();
        argument_infos = f.get_arguments();
    } else {
        context.error("Undefined variable {}", callee);
    }

    const auto& argument_types = callee_type->get_argument_types();

    vector<FunctionArgument> arguments;
    arguments.reserve(argument_types.size());

    if (!argument_infos.empty()) {
        // Check keyword arguments exist
        for (const auto& keyword : keyword_arguments | std::views::keys) {
            for (const auto& arg_info : argument_infos) {
                if (arg_info.key == keyword) { goto next; }
            }

            context.error("Unknown keyword argument {}", keyword);

        next:;
        }

        for (std::size_t i = 0; i < positional_arguments.size(); ++i) {
            utils::TraceBlock nnblock(
                context,
                "Checking the positional argument at index {}",
                i);

            const auto& arg_info = argument_infos[i];
            const auto& arg_type = argument_types[i];
            const auto& pos_arg = *positional_arguments[i];

            if (keyword_arguments.contains(arg_info.key)) {
                context.error(
                    "The argument '{}' is defined by the positional argument "
                    "at index {} and by a keyword argument.",
                    arg_info.key,
                    i);
            }

            DecoratedASTNodePtr decorated_arg =
                decorate_and_convert(pos_arg, *arg_type, context, env);

            arguments.emplace_back(move(decorated_arg), false);
        }

        for (std::size_t i = positional_arguments.size();
             i < argument_infos.size();
             ++i) {
            utils::TraceBlock nnblock(
                context,
                "Checking the positional argument at index {}",
                i);
            const auto& arg_info = argument_infos[i];
            const auto& arg_type = argument_types[i];

            if (auto it = keyword_arguments.find(arg_info.key);
                it != keyword_arguments.end()) {
                const auto& keyword_arg = it->second;

                DecoratedASTNodePtr decorated_arg =
                    decorate_and_convert(*keyword_arg, *arg_type, context, env);

                arguments.emplace_back(move(decorated_arg), false);
            } else if (arg_info.has_default()) {
                ASTNodePtr default_arg = [&] {
                    utils::TraceBlock nnnblock(
                        context,
                        "Parsing default value");
                    return tokenize_and_parse(arg_info.default_value);
                }();

                DecoratedASTNodePtr decorated_arg =
                    decorate_and_convert(*default_arg, *arg_type, context, env);

                arguments.emplace_back(move(decorated_arg), true);
            } else {
                context.error(
                    "Missing argument '{}' is mandatory.",
                    arg_info.key);
            }
        }
    } else {
        if (!keyword_arguments.empty()) {
            context.error("Keyword arguments not allowed here");
        }

        if (positional_arguments.size() != argument_types.size()) {
            context.error(
                "Expected {} arguments, {} were given",
                argument_types.size(),
                positional_arguments.size());
        }

        for (std::size_t i = 0; i < positional_arguments.size(); ++i) {
            utils::TraceBlock nnblock(
                context,
                "Checking the positional argument at index {}",
                i);

            const auto& arg = positional_arguments[i];
            const auto& arg_type = argument_types[i];

            DecoratedASTNodePtr decorated_arg =
                decorate_and_convert(*arg, *arg_type, context, env);

            arguments.emplace_back(move(decorated_arg), false);
        }
    }

    return {
        std::make_unique<DecoratedFunctionCallNode>(
            std::move(callee_node),
            move(arguments),
            unparsed_config),
        &callee_type->get_return_type()};
}

void DirectFunctionCallNode::dump(string indent) const
{
    cout << indent << "INDIRECTFUNC: ";
    std::print(cout, "{}", callee);
    cout << endl;
    indent = "| " + indent;
    cout << indent << "POSITIONAL ARGS:" << endl;
    for (const ASTNodePtr& node : positional_arguments) {
        node->dump("| " + indent);
    }
    cout << indent << "KEYWORD ARGS:" << endl;
    for (const auto& [key, default_arg] : keyword_arguments) {
        cout << indent << key << " = " << endl;
        default_arg->dump("| " + indent);
    }
}

} // namespace downward::cli::parser
