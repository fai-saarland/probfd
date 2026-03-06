#ifndef LANGUAGE_AST_LIST_NODE_H
#define LANGUAGE_AST_LIST_NODE_H

#include "language/ast/expression_node.h"

#include <memory>
#include <vector>

namespace language::parser {

class ListNode : public ExpressionNode {
    std::vector<std::unique_ptr<ExpressionNode>> elements;

public:
    explicit ListNode(std::vector<std::unique_ptr<ExpressionNode>>&& elements);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& type_registry) const override;
};

} // namespace language::parser

#endif
