#ifndef LANGUAGE_AST_LIST_NODE_H
#define LANGUAGE_AST_LIST_NODE_H

#include "language/ast/expression_node.h"

#include <memory>
#include <vector>

namespace language::parser {

class ListNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> elements;

public:
    explicit ListNode(std::vector<std::unique_ptr<ASTNode>>&& elements);

    TypedDecoratedAstNodePtr
    static_analysis(Context& context, VariableEnvironment& env) const override;
};

} // namespace language::parser

#endif
