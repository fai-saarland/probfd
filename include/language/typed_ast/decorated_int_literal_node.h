#ifndef LANGUAGE_DECORATED_INT_LITERAL_NODE_H
#define LANGUAGE_DECORATED_INT_LITERAL_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

namespace language::typed_ast {

class IntLiteralNode : public DecoratedExpressionNode {
    int value;

public:
    explicit IntLiteralNode(int value);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser
#endif
