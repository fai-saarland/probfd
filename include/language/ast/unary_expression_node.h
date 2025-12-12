#ifndef LANGUAGE_AST_UNARY_EXPRESSION_NODE_H
#define LANGUAGE_AST_UNARY_EXPRESSION_NODE_H

#include "language/ast/expression_node.h"

#include "language/token.h"

#include <memory>
#include <string>

namespace language::parser {

class UnaryNode : public ASTNode {
    ASTNodePtr nested_expr;
    TokenType token_type;

public:
    UnaryNode(ASTNodePtr nested_expr, const TokenType& token_type);

    TypedDecoratedAstNodePtr
    static_analysis(Context& context, VariableEnvironment& env)
        const override;
};

} // namespace language::parser

#endif
