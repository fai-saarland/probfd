#ifndef LANGUAGE_TYPED_AST_DECORATED_LET_NODE_H
#define LANGUAGE_TYPED_AST_DECORATED_LET_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

#include <memory>
#include <vector>

namespace language::typed_ast {
struct LocalValueDefinition;
}

namespace language::typed_ast {

class DecoratedLetNode : public DecoratedExpressionNode {
    std::vector<std::unique_ptr<LocalValueDefinition>>
        decorated_variable_definitions;
    std::unique_ptr<DecoratedExpressionNode> nested_value;

public:
    DecoratedLetNode(
        std::vector<std::unique_ptr<LocalValueDefinition>>
            decorated_variable_definitions,
        std::unique_ptr<DecoratedExpressionNode> nested_value);

    ~DecoratedLetNode() override;

    DecoratedLetNode(DecoratedLetNode&&) noexcept;
    DecoratedLetNode& operator=(DecoratedLetNode&&) noexcept;

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser

#endif
