#ifndef LANGUAGE_AST_LAMBDA_NODE_H
#define LANGUAGE_AST_LAMBDA_NODE_H

#include "language/ast/expression_node.h"

#include "declaration.h"

#include <memory>
#include <string>
#include <vector>

namespace language::parser {
class TypeNode;
}

namespace language::parser {

struct TypedParameter : Declaration {
    std::unique_ptr<TypeNode> type_node;

    TypedParameter(
        std::string parameter_name,
        std::unique_ptr<TypeNode> type_node);

    TypedParameter(TypedParameter&& other) noexcept;
    TypedParameter& operator=(TypedParameter&& other) noexcept;

    ~TypedParameter();
};

class LambdaNode : public ExpressionNode {
    std::vector<TypedParameter> parameters;
    std::unique_ptr<ExpressionNode> nested_value;

public:
    LambdaNode(
        std::vector<TypedParameter> parameters,
        std::unique_ptr<ExpressionNode> nested_value);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& registry) const override;
};

} // namespace language::parser

#endif
