#include "language/ast/unary_expression_node.h"

#include "language/typed_ast/decorated_unary_expression_node.h"

#include "language/typed_ast/type_registry.h"

#include "language/context.h"

using namespace std;

namespace language::parser {

UnaryNode::UnaryNode(
    std::unique_ptr<ExpressionNode> nested_expr,
    const TokenType& token_type)
    : nested_expr(std::move(nested_expr))
    , token_type(token_type)
{
}

TypedDecoratedAstNodePtr UnaryNode::static_analysis(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry& type_registry) const
{
    auto [ast_node, type] =
        nested_expr->static_analysis(context, env, local_env, type_registry);

    if (type == &type_registry.get_type<int>()) {
        return {
            std::make_unique<typed_ast::DecoratedUnaryExpressionNode<int>>(
                std::move(ast_node),
                token_type),
            type};
    }

    if (type == &type_registry.get_type<double>()) {
        return {
            std::make_unique<typed_ast::DecoratedUnaryExpressionNode<double>>(
                std::move(ast_node),
                token_type),
            type};
    }

    context.error(
        "Operator of unary arithmetic expression is not of numeric type.");
}

} // namespace language::parser
