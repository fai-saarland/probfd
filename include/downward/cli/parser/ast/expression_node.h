#ifndef PARSER_EXPRESSION_NODE_H
#define PARSER_EXPRESSION_NODE_H

#include <memory>
#include <string>

namespace downward::utils {
class Context;
}

namespace downward::cli::plugins {
class Registry;
class Type;
}

namespace downward::cli::parser {
class VariableEnvironment;
class DecoratedASTNode;
}

namespace downward::cli::parser {

struct TypedDecoratedAstNodePtr {
    std::unique_ptr<DecoratedASTNode> ast_node;
    const plugins::Type* type;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;

    std::unique_ptr<DecoratedASTNode>
    static_analysis(const plugins::Registry& registry) const;

    virtual TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

} // namespace downward::cli::parser

#endif
