#ifndef LANGUAGE_AST_LET_NODE_H
#define LANGUAGE_AST_LET_NODE_H

#include "language/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace downward::cli::parser {

class LetNode : public ASTNode {
    std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions;
    ASTNodePtr nested_value;

public:
    LetNode(
        std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions,
        ASTNodePtr nested_value);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;
};

} // namespace downward::cli::parser

#endif
