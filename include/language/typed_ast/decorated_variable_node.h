#ifndef LANGUAGE_DECORATED_VARIABLE_NODE_H
#define LANGUAGE_DECORATED_VARIABLE_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

namespace language::typed_ast {
class StackValue;
}

namespace language::typed_ast {

class VariableNode : public DecoratedExpressionNode {
    StackValue* value;

public:
    explicit VariableNode(StackValue& value);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser
#endif
