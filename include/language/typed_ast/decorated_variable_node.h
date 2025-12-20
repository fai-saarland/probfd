#ifndef LANGUAGE_DECORATED_VARIABLE_NODE_H
#define LANGUAGE_DECORATED_VARIABLE_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

namespace language::parser {
struct VariableDeclaration;
}

namespace language::parser {

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

} // namespace language::parser
#endif
