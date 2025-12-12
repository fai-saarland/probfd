#ifndef LANGUAGE_AST_DIRECT_FUNCTION_CALL_NODE_H
#define LANGUAGE_AST_DIRECT_FUNCTION_CALL_NODE_H

#include "language/ast/expression_node.h"
#include "language/ast/qualified_name.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace language::parser {

class DirectFunctionCallNode : public ASTNode {
    QualifiedName callee;
    std::vector<std::unique_ptr<ASTNode>> positional_arguments;
    std::unordered_map<std::string, std::unique_ptr<ASTNode>> keyword_arguments;
    std::string unparsed_config;

public:
    DirectFunctionCallNode(
        QualifiedName callee,
        std::vector<std::unique_ptr<ASTNode>>&& positional_arguments,
        std::unordered_map<std::string, std::unique_ptr<ASTNode>>&&
            keyword_arguments,
        const std::string& unparsed_config);

    TypedDecoratedAstNodePtr
    static_analysis(Context& context, VariableEnvironment& env) const override;
};

} // namespace language::parser

#endif
