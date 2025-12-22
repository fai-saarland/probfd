#ifndef LANGUAGE_AST_LITERAL_NODE_H
#define LANGUAGE_AST_LITERAL_NODE_H

#include "language/ast/expression_node.h"

#include "language/token.h"

namespace language::parser {

class LiteralNode : public ASTNode {
    Token value;

public:
    explicit LiteralNode(const Token& value);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        VariableEnvironment& env,
        plugins::TypeRegistry& type_registry) const override;
};

} // namespace language::parser

#endif
