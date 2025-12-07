#include "language/ast/unary_expression_node.h"

#include "language/decorated_abstract_syntax_tree.h"

#include "language/plugins/plugin.h"
#include "language/plugins/types.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::cli::parser {

UnaryNode::UnaryNode(ASTNodePtr nested_expr, const TokenType& token_type)
    : nested_expr(std::move(nested_expr))
    , token_type(token_type)
{
}

TypedDecoratedAstNodePtr
UnaryNode::static_analysis(utils::Context& context, VariableEnvironment& env)
    const
{
    auto [ast_node, type] = nested_expr->static_analysis(context, env);

    if (type == &plugins::TypeRegistry::instance()->get_type<int>()) {
        return {
            std::make_unique<DecoratedUnaryExpressionNode<int>>(
                std::move(ast_node),
                token_type),
            type};
    }

    if (type == &plugins::TypeRegistry::instance()->get_type<double>()) {
        return {
            std::make_unique<DecoratedUnaryExpressionNode<double>>(
                std::move(ast_node),
                token_type),
            type};
    }

    context.error(
        "Operator of unary arithmetic expression is not of numeric type.");
}

} // namespace downward::cli::parser
