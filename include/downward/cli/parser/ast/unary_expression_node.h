#ifndef PARSER_UNARY_EXPRESSION_NODE_H
#define PARSER_UNARY_EXPRESSION_NODE_H

#include "downward/cli/parser/ast/expression_node.h"

#include "downward/cli/parser/token.h"

#include <memory>
#include <string>

namespace downward::cli::parser {

class UnaryNode : public ASTNode {
    ASTNodePtr nested_expr;
    TokenType token_type;

public:
    UnaryNode(ASTNodePtr nested_expr, const TokenType& token_type);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;
};

} // namespace downward::cli::parser

#endif
