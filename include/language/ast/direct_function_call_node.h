#ifndef LANGUAGE_AST_DIRECT_FUNCTION_CALL_NODE_H
#define LANGUAGE_AST_DIRECT_FUNCTION_CALL_NODE_H

#include "language/ast/expression_node.h"
#include "language/ast/qualified_name.h"

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
};

} // namespace downward::cli::parser

#endif
