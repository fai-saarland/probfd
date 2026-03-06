#ifndef LANGUAGE_DECORATED_FUNCTION_CALL_NODE_H
#define LANGUAGE_DECORATED_FUNCTION_CALL_NODE_H

#include "language/typed_ast/decorated_expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace language::typed_ast {

class FunctionArgument {
    std::unique_ptr<DecoratedExpressionNode> value;
    bool is_default;

public:
    FunctionArgument(std::unique_ptr<DecoratedExpressionNode> value, bool is_default);

    DecoratedExpressionNode& get_value();
    const DecoratedExpressionNode& get_value() const;
    bool is_default_argument() const;
};

class DecoratedFunctionCallNode : public DecoratedExpressionNode {
    std::unique_ptr<DecoratedExpressionNode> callee;
    std::vector<FunctionArgument> arguments;

public:
    DecoratedFunctionCallNode(
        std::unique_ptr<DecoratedExpressionNode> callee,
        std::vector<FunctionArgument> arguments);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser
#endif
