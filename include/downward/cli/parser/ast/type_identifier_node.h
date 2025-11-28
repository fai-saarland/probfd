#ifndef PARSER_TYPE_IDENTIFIER_NODE_H
#define PARSER_TYPE_IDENTIFIER_NODE_H

#include "downward/cli/parser/ast/type_node.h"

#include "downward/cli/parser/ast/qualified_name.h"

namespace downward::cli::parser {

class TypeIdentifierNode : public TypeNode {
    QualifiedName name;

public:
    explicit TypeIdentifierNode(QualifiedName name);

    const plugins::Type&
    get_type(utils::Context& context, plugins::TypeRegistry& type_registry)
        const override;
};

} // namespace downward::cli::parser

#endif
