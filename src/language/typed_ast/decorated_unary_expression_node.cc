#include "language/typed_ast/decorated_unary_expression_node.h"

#include "language/typed_ast/construct_context.h"

#include <ranges>

using namespace std;

namespace language::typed_ast {

template <typename T>
    requires std::same_as<T, int> || std::same_as<T, double>
DecoratedUnaryExpressionNode<T>::DecoratedUnaryExpressionNode(
    std::unique_ptr<DecoratedExpressionNode> nested_expr,
    parser::TokenType token_type)
    : nested_expr(std::move(nested_expr))
    , token_type(std::move(token_type))
{
}

template <typename T>
    requires std::same_as<T, int> || std::same_as<T, double>
std::any
DecoratedUnaryExpressionNode<T>::construct(ConstructContext& context) const
{
    switch (token_type) {
    case parser::TokenType::PLUS: return nested_expr->construct(context);
    case parser::TokenType::MINUS:
        return -std::any_cast<T>(nested_expr->construct(context));
    default: context.error("Illegal unary expression operator: {}", token_type);
    }
}

template class DecoratedUnaryExpressionNode<int>;
template class DecoratedUnaryExpressionNode<double>;

} // namespace language::parser