#ifndef LANGUAGE_DECORATED_UNARY_EXPRESSION_NODE_H
#define LANGUAGE_DECORATED_UNARY_EXPRESSION_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

#include "language/token.h"

#include <memory>

namespace language::typed_ast {

template <typename T>
    requires std::same_as<T, int> || std::same_as<T, double>
class DecoratedUnaryExpressionNode : public DecoratedExpressionNode {
    std::unique_ptr<DecoratedExpressionNode> nested_expr;
    parser::TokenType token_type;

public:
    DecoratedUnaryExpressionNode(
        std::unique_ptr<DecoratedExpressionNode> nested_expr,
        parser::TokenType token_type);

    std::any construct(ConstructContext& context) const override;
};

extern template class DecoratedUnaryExpressionNode<int>;
extern template class DecoratedUnaryExpressionNode<double>;

} // namespace language::parser
#endif
