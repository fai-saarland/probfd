#ifndef LANGUAGE_AST_TYPE_LITERAL_NODE_H
#define LANGUAGE_AST_TYPE_LITERAL_NODE_H

#include "language/ast/type_node.h"

#include "language/token.h"

namespace language::parser {

class TypeLiteralNode : public TypeNode {
    Token value;

public:
    explicit TypeLiteralNode(const Token& value);

    const typed_ast::Type& get_type(
        Context& context,
        const typed_ast::GlobalEnvironment& environment,
        typed_ast::TypeRegistry& type_registry) const override;
};

} // namespace language::parser

#endif
