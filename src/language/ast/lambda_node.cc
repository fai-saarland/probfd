#include "language/ast/lambda_node.h"

#include "language/ast/type_node.h"
#include "language/ast/variable_environment.h"

#include "language/typed_ast/decorated_lambda_node.h"
#include "language/typed_ast/variable_definition.h"

#include "language/plugins/plugin.h"
#include "language/plugins/types.h"

#include "downward/utils/logging.h"

#include <vector>

using namespace std;

namespace language::parser {

TypedParameter::TypedParameter(
    std::string parameter_name,
    std::unique_ptr<TypeNode> type_node)
    : parameter_name(std::move(parameter_name))
    , type_node(std::move(type_node))
{
}

TypedParameter::TypedParameter(TypedParameter&& other) noexcept = default;

TypedParameter&
TypedParameter::operator=(TypedParameter&& other) noexcept = default;

TypedParameter::~TypedParameter() = default;

LambdaNode::LambdaNode(
    std::vector<TypedParameter> parameters,
    ASTNodePtr nested_value)
    : parameters(std::move(parameters))
    , nested_value(std::move(nested_value))
{
}

TypedDecoratedAstNodePtr
LambdaNode::static_analysis(Context& context, VariableEnvironment& env)
    const
{
    TraceBlock lblock(context, "Checking Lambda");

    std::vector<const plugins::Type*> arg_types;
    VariableEnvironment nested_env(env.get_registry());

    std::vector<VariableDeclaration> decorated_variable_declarations;
    decorated_variable_declarations.reserve(parameters.size());

    std::size_t i = 1;
    for (auto& [variable_name, type_node] : parameters) {
        TraceBlock block(context, "Checking Parameter #{}", i++);

        auto& param_declaration =
            decorated_variable_declarations.emplace_back(variable_name);
        const auto& t =
            type_node->get_type(context, *plugins::TypeRegistry::instance());
        arg_types.emplace_back(&t);
        const bool s =
            nested_env.add_variable(variable_name, t, param_declaration);

        if (!s) {
            context.error(
                "A parameter with name '{}' already exists.",
                variable_name);
        }
    }

    auto [ast_node, rtype] = [&] {
        TraceBlock block(context, "Checking Body.");
        return nested_value->static_analysis(context, nested_env);
    }();

    const auto& ftype = plugins::TypeRegistry::instance()->create_function_type(
        *rtype,
        std::move(arg_types));

    return {
        std::make_unique<DecoratedLambdaNode>(
            ftype,
            std::move(decorated_variable_declarations),
            std::move(ast_node)),
        &ftype};
}

} // namespace language::parser
