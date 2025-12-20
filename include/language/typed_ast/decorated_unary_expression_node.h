#ifndef LANGUAGE_DECORATED_UNARY_EXPRESSION_NODE_H
#define LANGUAGE_DECORATED_UNARY_EXPRESSION_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

#include "language/token.h"

#include <memory>

namespace language::parser {

template <typename T>
    requires std::same_as<T, int> || std::same_as<T, double>
class DecoratedUnaryExpressionNode : public DecoratedASTNode {
    std::unique_ptr<DecoratedASTNode> nested_expr;
    TokenType token_type;

public:
    DecoratedUnaryExpressionNode(
        std::unique_ptr<DecoratedASTNode> nested_expr,
        TokenType token_type);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

extern template class DecoratedUnaryExpressionNode<int>;
extern template class DecoratedUnaryExpressionNode<double>;

} // namespace language::parser
#endif
