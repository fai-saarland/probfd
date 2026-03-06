#include "language/ast/lambda_node.h"

#include "language/ast/internal_function_definition.h"
#include "language/ast/type_node.h"

#include "language/typed_ast/decorated_lambda_node.h"
#include "language/typed_ast/decorated_variable_node.h"
#include "language/typed_ast/variable_environment.h"

#include "language/typed_ast/types.h"

#include <vector>

using namespace std;

namespace language::parser {

TypedParameter::TypedParameter(
    std::string parameter_name,
    std::unique_ptr<TypeNode> type_node)
    : Declaration(std::move(parameter_name))
    , type_node(std::move(type_node))
{
}

TypedParameter::TypedParameter(TypedParameter&& other) noexcept = default;

TypedParameter&
TypedParameter::operator=(TypedParameter&& other) noexcept = default;

TypedParameter::~TypedParameter() = default;

LambdaNode::LambdaNode(
    std::vector<TypedParameter> parameters,
    std::unique_ptr<ExpressionNode> nested_value)
    : parameters(std::move(parameters))
    , nested_value(std::move(nested_value))
{
}

TypedDecoratedAstNodePtr LambdaNode::static_analysis(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment&,
    typed_ast::TypeRegistry& type_registry) const
{
    TraceBlock lblock(context, "Checking Lambda");

    Context nested_context;

    typed_ast::LocalEnvironment l_env;

    std::vector<typed_ast::LambdaParameterValue>
        decorated_variable_declarations;
    decorated_variable_declarations.reserve(parameters.size());

    std::vector<const typed_ast::Type*> arg_types;

    for (std::size_t i = 0; const auto& param : parameters) {
        TraceBlock block(context, "Checking Parameter #{}", i++);

        auto& param_declaration = decorated_variable_declarations.emplace_back(
            l_env.get_num_local_variables());

        const auto& t = param.type_node->get_type(context, env, type_registry);
        arg_types.emplace_back(&t);

        if (!l_env.add_variable(param.get_identifier(), t, param_declaration)) {
            context.error(
                "A parameter with name '{}' already exists.",
                param.get_identifier());
        }
    }

    auto [ast_node, rtype] = [&] {
        TraceBlock block(context, "Checking Body.");
        return nested_value
            ->static_analysis(context, env, l_env, type_registry);
    }();

    const auto& ftype =
        type_registry.create_function_type(*rtype, std::move(arg_types));

    return {
        std::make_unique<typed_ast::DecoratedLambdaNode>(
            ftype,
            std::move(decorated_variable_declarations),
            std::move(ast_node)),
        &ftype};
}

} // namespace language::parser
