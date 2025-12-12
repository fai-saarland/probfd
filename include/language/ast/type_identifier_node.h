#ifndef LANGUAGE_AST_TYPE_IDENTIFIER_NODE_H
#define LANGUAGE_AST_TYPE_IDENTIFIER_NODE_H

#include "language/ast/type_node.h"

#include "language/ast/qualified_name.h"

namespace language::parser {

class TypeIdentifierNode : public TypeNode {
    QualifiedName name;

public:
    explicit TypeIdentifierNode(QualifiedName name);

    const plugins::Type&
    get_type(Context& context, plugins::TypeRegistry& type_registry)
        const override;
};

} // namespace downward::cli::parser

#endif
