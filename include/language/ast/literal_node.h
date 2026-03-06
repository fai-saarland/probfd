#ifndef LANGUAGE_AST_LITERAL_NODE_H
#define LANGUAGE_AST_LITERAL_NODE_H

#include "language/ast/expression_node.h"

#include "language/token.h"

namespace language::parser {

class LiteralNode : public ExpressionNode {
    Token value;

public:
    explicit LiteralNode(const Token& value);

    TypedDecoratedAstNodePtr static_analysis(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::LocalEnvironment& local_env,
        typed_ast::TypeRegistry& type_registry) const override;
};

} // namespace language::parser

#endif
