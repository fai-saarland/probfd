#ifndef LANGUAGE_AST_INDIRECT_FUNCTION_CALL_NODE_H
#define LANGUAGE_AST_INDIRECT_FUNCTION_CALL_NODE_H

#include "language/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace language::parser {

class IndirectFunctionCallNode : public ASTNode {
    ASTNodePtr callee;
    std::vector<ASTNodePtr> positional_arguments;
    std::string unparsed_config;

public:
    IndirectFunctionCallNode(
        ASTNodePtr callee,
        std::vector<ASTNodePtr>&& positional_arguments,
        const std::string& unparsed_config);

    TypedDecoratedAstNodePtr
    static_analysis(Context& context, VariableEnvironment& env) const override;
};

} // namespace language::parser

#endif
