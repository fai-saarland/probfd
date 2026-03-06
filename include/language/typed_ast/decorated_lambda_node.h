#ifndef LANGUAGE_DECORATED_LAMBDA_NODE_H
#define LANGUAGE_DECORATED_LAMBDA_NODE_H

#include "language/ast/declaration.h"

#include "language/typed_ast/decorated_expression_node.h"
#include "language/typed_ast/variable_environment.h"

#include <any>
#include <memory>
#include <vector>

namespace language::typed_ast {
class VariableNode;
struct TypedParameter;
class Declaration;
} // namespace language::parser

namespace language::typed_ast {

class LambdaParameterValue : public StackValue {
public:
    using StackValue::StackValue;

    std::unique_ptr<DecoratedExpressionNode> create_load_node() override;
};

class DecoratedLambdaNode : public DecoratedExpressionNode {
    const FunctionType& type;
    std::vector<LambdaParameterValue> decorated_variable_declarations;
    std::unique_ptr<DecoratedExpressionNode> nested_value;

public:
    DecoratedLambdaNode(
        const FunctionType& type,
        std::vector<LambdaParameterValue> decorated_variable_declarations,
        std::unique_ptr<DecoratedExpressionNode> nested_value);

    ~DecoratedLambdaNode() override;

    DecoratedLambdaNode(DecoratedLambdaNode&&) noexcept;

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser
#endif
