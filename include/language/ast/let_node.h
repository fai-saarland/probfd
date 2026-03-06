#ifndef LANGUAGE_AST_LET_NODE_H
#define LANGUAGE_AST_LET_NODE_H

#include "language/ast/expression_node.h"

#include "declaration.h"

#include <memory>
#include <vector>

namespace language::parser {

struct LetNodeDefinition : public Declaration {
    std::unique_ptr<ExpressionNode> expression;

public:
    LetNodeDefinition(
        std::string identifier,
        std::unique_ptr<ExpressionNode> expression);

    const ExpressionNode& get_expression() const;
};

class LetNode : public ExpressionNode {
    std::vector<LetNodeDefinition> variable_definitions;
    std::unique_ptr<ExpressionNode> nested_value;

public:
    LetNode(
        std::vector<LetNodeDefinition> variable_definitions,
        std::unique_ptr<ExpressionNode> nested_value);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& type_registry) const override;
};

} // namespace language::parser

#endif
