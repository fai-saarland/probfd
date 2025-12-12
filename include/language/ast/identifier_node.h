#ifndef LANGUAGE_AST_IDENTIFIER_NODE_H
#define LANGUAGE_AST_IDENTIFIER_NODE_H

#include "language/ast/expression_node.h"
#include "language/ast/qualified_name.h"

namespace language::parser {

class IdentifierNode : public ASTNode {
    QualifiedName qualified_name;

public:
    explicit IdentifierNode(QualifiedName qualified_name);

    TypedDecoratedAstNodePtr
    static_analysis(Context& context, VariableEnvironment& env) const override;

    const QualifiedName& get_name() const;
};

} // namespace downward::cli::parser

#endif
