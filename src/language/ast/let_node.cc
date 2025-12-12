#include "language/ast/let_node.h"

#include "language/ast/variable_environment.h"

#include "language/typed_ast/decorated_let_node.h"
#include "language/typed_ast/variable_definition.h"

#include "language/plugins/plugin.h"

#include "downward/utils/logging.h"
#include "downward/utils/strings.h"

#include <set>
#include <vector>

using namespace std;

namespace language::parser {

LetNode::LetNode(
    std::vector<std::pair<std::string, std::unique_ptr<ASTNode>>> variable_definitions,
    std::unique_ptr<ASTNode> nested_value)
    : variable_definitions(move(variable_definitions))
    , nested_value(move(nested_value))
{
}

TypedDecoratedAstNodePtr
LetNode::static_analysis(Context& context, VariableEnvironment& env)
    const
{
    TraceBlock lblock(
        context,
        "Checking Let: {:n:s}",
        variable_definitions | views::keys);

    std::vector<VariableDefinition> decorated_variable_definitions;
    decorated_variable_definitions.reserve(variable_definitions.size());

    env.enter_scope();

    for (const auto& [variable_name, variable_definition] :
         variable_definitions) {
        TraceBlock block(context, "Check variable definition");
        auto [ast_node, type] =
            variable_definition->static_analysis(context, env);
        auto& declaration = decorated_variable_definitions.emplace_back(
            variable_name,
            std::move(ast_node));

        env.add_variable(context, variable_name, *type, declaration);
    }

    TypedDecoratedAstNodePtr decorated_nested_value;

    {
        TraceBlock block(context, "Check nested expression.");
        decorated_nested_value = nested_value->static_analysis(context, env);
    }

    env.leave_scope();

    return {
        std::make_unique<DecoratedLetNode>(
            move(decorated_variable_definitions),
            move(decorated_nested_value.ast_node)),
        decorated_nested_value.type};
}

} // namespace language::parser
