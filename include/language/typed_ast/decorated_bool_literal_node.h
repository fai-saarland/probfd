#ifndef LANGUAGE_DECORATED_BOOL_LITERAL_NODE_H
#define LANGUAGE_DECORATED_BOOL_LITERAL_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

namespace language::typed_ast {

class BoolLiteralNode : public DecoratedExpressionNode {
    bool value;

public:
    explicit BoolLiteralNode(bool value);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser
#endif
