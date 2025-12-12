#ifndef LANGUAGE_DECORATED_UNARY_EXPRESSION_NODE_H
#define LANGUAGE_DECORATED_UNARY_EXPRESSION_NODE_H

#include "language/token.h"

#include "language/typed_ast/decorated_ast_node.h"

namespace language::parser {

template <typename T>
    requires std::same_as<T, int> || std::same_as<T, double>
class DecoratedUnaryExpressionNode : public DecoratedASTNode {
    DecoratedASTNodePtr nested_expr;
    TokenType token_type;

public:
    DecoratedUnaryExpressionNode(
        DecoratedASTNodePtr nested_expr,
        TokenType token_type);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

extern template class DecoratedUnaryExpressionNode<int>;
extern template class DecoratedUnaryExpressionNode<double>;

} // namespace downward::cli::parser
#endif
