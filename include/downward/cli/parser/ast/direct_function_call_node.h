#ifndef PARSER_DIRECT_FUNCTION_CALL_NODE_H
#define PARSER_DIRECT_FUNCTION_CALL_NODE_H

#include "downward/cli/parser/ast/expression_node.h"
#include "downward/cli/parser/ast/qualified_name.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace downward::cli::parser {

class DirectFunctionCallNode : public ASTNode {
    QualifiedName callee;
    std::vector<ASTNodePtr> positional_arguments;
    std::unordered_map<std::string, ASTNodePtr> keyword_arguments;
    std::string unparsed_config;

public:
    DirectFunctionCallNode(
        QualifiedName callee,
        std::vector<ASTNodePtr>&& positional_arguments,
        std::unordered_map<std::string, ASTNodePtr>&& keyword_arguments,
        const std::string& unparsed_config);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

} // namespace downward::cli::parser

#endif
