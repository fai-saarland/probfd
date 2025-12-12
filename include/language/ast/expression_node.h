#ifndef LANGUAGE_AST_EXPRESSION_NODE_H
#define LANGUAGE_AST_EXPRESSION_NODE_H

#include <memory>
#include <string>

namespace language {
class Context;
}

namespace language::plugins {
class Registry;
class Type;
} // namespace language::plugins

namespace language::parser {
class VariableEnvironment;
class DecoratedASTNode;
} // namespace language::parser

namespace language::parser {

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
    static_analysis(Context& context, VariableEnvironment& env) const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

} // namespace language::parser

#endif
