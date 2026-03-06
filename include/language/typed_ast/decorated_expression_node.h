#ifndef LANGUAGE_DECORATED_EXPRESSION_NODE_H
#define LANGUAGE_DECORATED_EXPRESSION_NODE_H

#include <any>
#include <cstddef>
#include <iostream>

namespace language::typed_ast {
class ConstructContext;
} // namespace language::parser

namespace language::typed_ast {

class DecoratedExpressionNode {
public:
    virtual ~DecoratedExpressionNode() = default;

    virtual std::any construct(ConstructContext& context) const = 0;
};

extern std::any construct(const DecoratedExpressionNode& node);

} // namespace language::parser
#endif
