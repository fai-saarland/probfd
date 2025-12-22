#ifndef LANGUAGE_TYPED_AST_DECORATED_LET_NODE_H
#define LANGUAGE_TYPED_AST_DECORATED_LET_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

#include "language/typed_ast/variable_declaration.h"

#include <any>
#include <memory>
#include <vector>

namespace language::parser {

struct VariableDefinition : VariableDeclaration {
    std::unique_ptr<DecoratedASTNode> variable_expression;

    VariableDefinition(
        std::string variable_name,
        std::unique_ptr<DecoratedASTNode> variable_expression);

    ~VariableDefinition() override;

    VariableDefinition(VariableDefinition&& other) noexcept;
    VariableDefinition& operator=(VariableDefinition&& other) noexcept;

    std::unique_ptr<DecoratedASTNode> create_load_node() override;
};

class DecoratedLetNode : public DecoratedASTNode {
    std::vector<std::unique_ptr<VariableDefinition>>
        decorated_variable_definitions;
    std::unique_ptr<DecoratedASTNode> nested_value;

public:
    DecoratedLetNode(
        std::vector<std::unique_ptr<VariableDefinition>>
            decorated_variable_definitions,
        std::unique_ptr<DecoratedASTNode> nested_value);

    ~DecoratedLetNode() override;

    DecoratedLetNode(DecoratedLetNode&&) = default;
    DecoratedLetNode& operator=(DecoratedLetNode&&) noexcept = default;

    void prune_unused_definitions(
        std::vector<std::unique_ptr<VariableDeclaration>>& defs) override;
    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace language::parser
#endif
