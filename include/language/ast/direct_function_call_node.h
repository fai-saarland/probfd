#ifndef LANGUAGE_AST_DIRECT_FUNCTION_CALL_NODE_H
#define LANGUAGE_AST_DIRECT_FUNCTION_CALL_NODE_H

#include "language/ast/expression_node.h"
#include "language/ast/qualified_name.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace language::typed_ast {
struct TypedFunctionValue;
struct TypedValue;
} // namespace language::parser

namespace language::parser {

class DirectFunctionCallNode : public ExpressionNode {
    QualifiedName callee;
    std::vector<std::unique_ptr<ExpressionNode>> positional_arguments;
    std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>
        keyword_arguments;

public:
    DirectFunctionCallNode(
        QualifiedName callee,
        std::vector<std::unique_ptr<ExpressionNode>>&& positional_arguments,
        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>&&
            keyword_arguments);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& type_registry) const override;

private:
    TypedDecoratedAstNodePtr construct_indirect_call(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& type_registry,
        const typed_ast::TypedValue& decl) const;

    TypedDecoratedAstNodePtr construct_call(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& type_registry,
        const std::vector<typed_ast::TypedFunctionValue>& decl) const;
};

} // namespace language::parser

#endif
