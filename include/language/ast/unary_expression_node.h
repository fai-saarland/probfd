#ifndef LANGUAGE_AST_UNARY_EXPRESSION_NODE_H
#define LANGUAGE_AST_UNARY_EXPRESSION_NODE_H

#include "language/ast/expression_node.h"

#include "language/token.h"

#include <memory>

namespace language::parser {

class UnaryNode : public ExpressionNode {
    std::unique_ptr<ExpressionNode> nested_expr;
    TokenType token_type;

public:
    UnaryNode(
        std::unique_ptr<ExpressionNode> nested_expr,
        const TokenType& token_type);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& type_registry) const override;
};

} // namespace language::parser

#endif
