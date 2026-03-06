#ifndef LANGUAGE_DECORATED_FLOAT_LITERAL_NODE_H
#define LANGUAGE_DECORATED_FLOAT_LITERAL_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

namespace language::typed_ast {

class FloatLiteralNode : public DecoratedExpressionNode {
    double value;

public:
    explicit FloatLiteralNode(double value);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser
#endif
