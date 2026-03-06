#include "language/ast/let_node.h"

#include "language/ast/internal_function_definition.h"

#include "language/typed_ast/decorated_let_node.h"
#include "language/typed_ast/local_value_definition.h"
#include "language/typed_ast/variable_environment.h"

#include <ranges>
#include <vector>

using namespace std;

namespace language::parser {

LetNodeDefinition::LetNodeDefinition(
    std::string identifier,
    std::unique_ptr<ExpressionNode> expression)
    : Declaration(std::move(identifier))
    , expression(std::move(expression))
{
}

const ExpressionNode& LetNodeDefinition::get_expression() const
{
    return *expression;
}

LetNode::LetNode(
    std::vector<LetNodeDefinition> variable_definitions,
    std::unique_ptr<ExpressionNode> nested_value)
    : variable_definitions(move(variable_definitions))
    , nested_value(move(nested_value))
{
}

TypedDecoratedAstNodePtr LetNode::static_analysis(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry& type_registry) const
{
    TraceBlock lblock(
        context,
        "Checking Let: {:n}",
        variable_definitions |
            views::transform(&LetNodeDefinition::get_identifier));

    std::vector<std::unique_ptr<typed_ast::LocalValueDefinition>>
        decorated_variable_definitions;
    decorated_variable_definitions.reserve(variable_definitions.size());

    local_env.enter_scope();

    for (const auto& var_def : variable_definitions) {
        TraceBlock block(context, "Check variable definition");
        auto [ast_node, type] = var_def.get_expression().static_analysis(
            context,
            env,
            local_env,
            type_registry);

        auto& value = decorated_variable_definitions.emplace_back(
            std::make_unique<typed_ast::LocalValueDefinition>(
                local_env.get_num_local_variables(),
                std::move(ast_node)));

        if (!local_env.add_variable(var_def.get_identifier(), *type, *value)) {
            context.error(
                "Variable '{}' has already been declared.",
                var_def.get_identifier());
        }
    }

    TypedDecoratedAstNodePtr decorated_nested_value;

    {
        TraceBlock block(context, "Check nested expression.");
        decorated_nested_value = nested_value->static_analysis(
            context,
            env,
            local_env,
            type_registry);
    }

    local_env.leave_scope();

    return {
        std::make_unique<typed_ast::DecoratedLetNode>(
            move(decorated_variable_definitions),
            move(decorated_nested_value.ast_node)),
        decorated_nested_value.type};
}

} // namespace language::parser
