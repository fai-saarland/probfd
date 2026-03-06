#ifndef LANGUAGE_DECORATED_FEATURE_LITERAL_NODE_H
#define LANGUAGE_DECORATED_FEATURE_LITERAL_NODE_H

#include "language/typed_ast/decorated_expression_node.h"
#include "language/typed_ast/type_aliases.h"

namespace language {
class Context;
}

namespace language::typed_ast {

class FeatureLiteralNode : public DecoratedExpressionNode {
    AnyFunctionType callee;

public:
    explicit FeatureLiteralNode(AnyFunctionType callee);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser
#endif
