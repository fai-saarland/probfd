#ifndef PARSER_LITERAL_NODE_TREE_H
#define PARSER_LITERAL_NODE_TREE_H

#include "downward/cli/parser/ast/expression_node.h"

#include "downward/cli/parser/token.h"

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
