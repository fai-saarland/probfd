#ifndef LANGUAGE_DECORATED_STRING_LITERAL_NODE_H
#define LANGUAGE_DECORATED_STRING_LITERAL_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

#include <string>

namespace language::typed_ast {

class StringLiteralNode : public DecoratedExpressionNode {
    std::string value;

public:
    explicit StringLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser
#endif
