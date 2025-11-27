#ifndef PARSER_TYPE_LITERAL_NODE_H
#define PARSER_TYPE_LITERAL_NODE_H

#include "downward/cli/parser/ast/type_node.h"

#include "downward/cli/parser/token.h"

#include <string>

namespace downward::cli::parser {

class TypeLiteralNode : public TypeNode {
    Token value;

public:
    explicit TypeLiteralNode(const Token& value);

    const plugins::Type&
    get_type(plugins::TypeRegistry& type_registry) const override;

    void dump(std::string indent) const override;
};

} // namespace downward::cli::parser

#endif
