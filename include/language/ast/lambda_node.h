#ifndef LANGUAGE_AST_LAMBDA_NODE_H
#define LANGUAGE_AST_LAMBDA_NODE_H

#include "language/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace language::parser {
class TypeNode;
}

namespace language::parser {

struct TypedParameter {
    std::string parameter_name;
    std::unique_ptr<TypeNode> type_node;

    TypedParameter(
        std::string parameter_name,
        std::unique_ptr<TypeNode> type_node);

    TypedParameter(TypedParameter&& other) noexcept;
    TypedParameter& operator=(TypedParameter&& other) noexcept;

    ~TypedParameter();
};

class LambdaNode : public ASTNode {
    std::vector<TypedParameter> parameters;
    std::unique_ptr<ASTNode> nested_value;

public:
    LambdaNode(
        std::vector<TypedParameter> parameters,
        std::unique_ptr<ASTNode> nested_value);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        VariableEnvironment& env,
        plugins::TypeRegistry& registry) const override;
};

} // namespace language::parser

#endif
