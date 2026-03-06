#ifndef LANGUAGE_AST_EXPRESSION_NODE_H
#define LANGUAGE_AST_EXPRESSION_NODE_H

#include <memory>

namespace language {
class Context;
}

namespace language::parser {
class CompilationContext;
} // namespace language::parser

namespace language::typed_ast {
class LocalEnvironment;
class GlobalEnvironment;
class DecoratedExpressionNode;
class Type;
class TypeRegistry;
} // namespace language::typed_ast

namespace language::parser {

struct TypedDecoratedAstNodePtr {
    std::unique_ptr<typed_ast::DecoratedExpressionNode> ast_node;
    const typed_ast::Type* type;
};

class ExpressionNode {
public:
    virtual ~ExpressionNode() = default;

    virtual TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& registry) const = 0;
};

std::unique_ptr<typed_ast::DecoratedExpressionNode>
static_analysis(const ExpressionNode& node, CompilationContext& registry);

} // namespace language::parser

#endif
