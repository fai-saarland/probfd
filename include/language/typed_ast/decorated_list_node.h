#ifndef LANGUAGE_DECORATED_LIST_NODE_H
#define LANGUAGE_DECORATED_LIST_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

#include <memory>
#include <vector>

namespace language::typed_ast {

class DecoratedListNode : public DecoratedExpressionNode {
    std::vector<std::unique_ptr<DecoratedExpressionNode>> elements;

public:
    explicit DecoratedListNode(
        std::vector<std::unique_ptr<DecoratedExpressionNode>>&& elements);

    std::any construct(ConstructContext& context) const override;

    const std::vector<std::unique_ptr<DecoratedExpressionNode>>&
    get_elements() const
    {
        return elements;
    }
};

} // namespace language::typed_ast
#endif
