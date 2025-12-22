#ifndef LANGUAGE_AST_LET_NODE_H
#define LANGUAGE_AST_LET_NODE_H

#include "language/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace language::parser {

struct LetNodeDefinition {
    std::string identifier;
    std::unique_ptr<ASTNode> expression;
};

class LetNode : public ASTNode {
    std::vector<LetNodeDefinition> variable_definitions;
    std::unique_ptr<ASTNode> nested_value;

public:
    LetNode(
        std::vector<LetNodeDefinition> variable_definitions,
        std::unique_ptr<ASTNode> nested_value);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        VariableEnvironment& env,
        plugins::TypeRegistry& type_registry) const override;
};

} // namespace language::parser

#endif
