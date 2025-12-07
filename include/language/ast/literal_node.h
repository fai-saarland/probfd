#ifndef LANGUAGE_AST_LITERAL_NODE_H
#define LANGUAGE_AST_LITERAL_NODE_H

#include "language/ast/expression_node.h"

#include "language/token.h"

#include <string>

namespace downward::cli::parser {

class LiteralNode : public ASTNode {
    Token value;

public:
    explicit LiteralNode(const Token& value);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;
};

} // namespace downward::cli::parser

#endif
