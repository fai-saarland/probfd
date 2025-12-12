#include "language/ast/unary_expression_node.h"

#include "language/typed_ast/decorated_unary_expression_node.h"

#include "language/plugins/types.h"

#include "language/context.h"

using namespace std;

namespace language::parser {

UnaryNode::UnaryNode(std::unique_ptr<ASTNode> nested_expr, const TokenType& token_type)
    : nested_expr(std::move(nested_expr))
    , token_type(token_type)
{
}

TypedDecoratedAstNodePtr
UnaryNode::static_analysis(Context& context, VariableEnvironment& env)
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

} // namespace language::parser
