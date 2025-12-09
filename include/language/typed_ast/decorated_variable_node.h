#ifndef LANGUAGE_DECORATED_VARIABLE_NODE_H
#define LANGUAGE_DECORATED_VARIABLE_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

namespace downward::cli::parser {
struct VariableDeclaration;
}

namespace downward::cli::parser {

class VariableNode : public DecoratedASTNode {
    friend VariableDeclaration;

    VariableDeclaration* declaration;

public:
    explicit VariableNode(VariableDeclaration& declaration);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace downward::cli::parser
#endif
