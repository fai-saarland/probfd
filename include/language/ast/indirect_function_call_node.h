#ifndef LANGUAGE_AST_INDIRECT_FUNCTION_CALL_NODE_H
#define LANGUAGE_AST_INDIRECT_FUNCTION_CALL_NODE_H

#include "language/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace language::parser {

class IndirectFunctionCallNode : public ExpressionNode {
    std::unique_ptr<ExpressionNode> callee;
    std::vector<std::unique_ptr<ExpressionNode>> positional_arguments;

public:
    IndirectFunctionCallNode(
        std::unique_ptr<ExpressionNode> callee,
        std::vector<std::unique_ptr<ExpressionNode>>&& positional_arguments);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& type_registry) const override;
};

} // namespace language::parser

#endif
