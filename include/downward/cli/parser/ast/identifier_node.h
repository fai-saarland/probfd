#ifndef PARSER_IDENTIFIER_NODE_H
#define PARSER_IDENTIFIER_NODE_H

#include "downward/cli/parser/ast/expression_node.h"
#include "downward/cli/parser/ast/qualified_name.h"

#include <string>

namespace downward::cli::parser {

class IdentifierNode : public ASTNode {
    QualifiedName qualified_name;

public:
    explicit IdentifierNode(QualifiedName qualified_name);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;

    const QualifiedName& get_name() const;
};

} // namespace downward::cli::parser

#endif
