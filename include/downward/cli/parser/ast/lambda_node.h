#ifndef PARSER_LAMBDA_NODE_H
#define PARSER_LAMBDA_NODE_H

#include "downward/cli/parser/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace downward::cli::parser {
class TypeNode;
}

namespace downward::cli::parser {

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
    ASTNodePtr nested_value;

public:
    LambdaNode(std::vector<TypedParameter> parameters, ASTNodePtr nested_value);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;
};

} // namespace downward::cli::parser

#endif
