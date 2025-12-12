#ifndef LANGUAGE_AST_LET_NODE_H
#define LANGUAGE_AST_LET_NODE_H

#include "language/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace language::parser {

class LetNode : public ASTNode {
    std::vector<std::pair<std::string, std::unique_ptr<ASTNode>>> variable_definitions;
    std::unique_ptr<ASTNode> nested_value;

public:
    LetNode(
        std::vector<std::pair<std::string, std::unique_ptr<ASTNode>>> variable_definitions,
        std::unique_ptr<ASTNode> nested_value);

    TypedDecoratedAstNodePtr
    static_analysis(Context& context, VariableEnvironment& env) const override;
};

} // namespace language::parser

#endif
