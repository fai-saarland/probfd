#ifndef LANGUAGE_DECORATED_ABSTRACT_SYNTAX_TREE_H
#define LANGUAGE_DECORATED_ABSTRACT_SYNTAX_TREE_H

#include "language/typed_ast/decorated_expression_node.h"

namespace language::typed_ast {
class Type;
}

namespace language::typed_ast {

class ConvertNode : public DecoratedExpressionNode {
    std::unique_ptr<DecoratedExpressionNode> value;
    const Type& from_type;
    const Type& to_type;

public:
    ConvertNode(
        std::unique_ptr<DecoratedExpressionNode> value,
        const Type& from_type,
        const Type& to_type);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::typed_ast
#endif
