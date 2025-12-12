#include "language/typed_ast/decorated_unary_expression_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/plugins/types.h"

#include "downward/utils/logging.h"

#include <ranges>

using namespace std;

namespace language::parser {

template <typename T>
    requires std::same_as<T, int> || std::same_as<T, double>
DecoratedUnaryExpressionNode<T>::DecoratedUnaryExpressionNode(
    std::unique_ptr<DecoratedASTNode> nested_expr,
    TokenType token_type)
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
    case TokenType::PLUS: return nested_expr->construct(context);
    case TokenType::MINUS:
        return -std::any_cast<T>(nested_expr->construct(context));
    default:
        context.error(
            "Illegal unary expression operator: {}",
            token_type);
    }
}

template <typename T>
    requires std::same_as<T, int> || std::same_as<T, double>
void DecoratedUnaryExpressionNode<T>::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", token_type);
    nested_expr->print(out, 0, print_default_args);
}

template class DecoratedUnaryExpressionNode<int>;
template class DecoratedUnaryExpressionNode<double>;

} // namespace language::parser