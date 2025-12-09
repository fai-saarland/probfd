#ifndef LANGUAGE_TYPED_AST_DECORATED_LET_NODE_H
#define LANGUAGE_TYPED_AST_DECORATED_LET_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

#include <any>
#include <vector>

namespace downward::cli::parser {
struct VariableDefinition;
}

namespace downward::cli::parser {

class DecoratedLetNode : public DecoratedASTNode {
    std::vector<VariableDefinition> decorated_variable_definitions;
    DecoratedASTNodePtr nested_value;

public:
    DecoratedLetNode(
        std::vector<VariableDefinition> decorated_variable_definitions,
        DecoratedASTNodePtr nested_value);

    ~DecoratedLetNode() override;

    DecoratedLetNode(DecoratedLetNode&&) = default;
    DecoratedLetNode& operator=(DecoratedLetNode&&) noexcept = default;

    void
    prune_unused_definitions(std::vector<VariableDefinition>& defs) override;
    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace downward::cli::parser
#endif
