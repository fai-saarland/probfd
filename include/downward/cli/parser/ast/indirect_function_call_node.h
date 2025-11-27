#ifndef PARSER_INDIRECT_FUNCTION_CALL_NODE_H
#define PARSER_INDIRECT_FUNCTION_CALL_NODE_H

#include "downward/cli/parser/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace downward::cli::parser {

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
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

} // namespace downward::cli::parser

#endif
