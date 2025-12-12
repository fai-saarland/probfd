#ifndef LANGUAGE_DECORATED_LIST_NODE_H
#define LANGUAGE_DECORATED_LIST_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

#include <memory>
#include <vector>

namespace language::parser {

class DecoratedListNode : public DecoratedASTNode {
    std::vector<std::unique_ptr<DecoratedASTNode>> elements;

public:
    explicit DecoratedListNode(std::vector<std::unique_ptr<DecoratedASTNode>>&& elements);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;

    const std::vector<std::unique_ptr<DecoratedASTNode>>& get_elements() const
    {
        return elements;
    }
};

} // namespace downward::cli::parser
#endif
