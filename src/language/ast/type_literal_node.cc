#include "language/ast/type_literal_node.h"

#include "language/plugins/type_registry.h"

#include "language/context.h"

using namespace std;

namespace language::parser {

TypeLiteralNode::TypeLiteralNode(const Token& value)
    : value(value)
{
}

const plugins::Type& TypeLiteralNode::get_type(
    Context& context,
    const VariableEnvironment&,
    plugins::TypeRegistry& type_registry) const
{
    switch (value.type) {
    case TokenType::TYPE_BOOL: return type_registry.get_type<bool>();
    case TokenType::TYPE_INTEGER: return type_registry.get_type<int>();
    case TokenType::TYPE_FLOAT: return type_registry.get_type<float>();
    case TokenType::TYPE_STRING: return type_registry.get_type<std::string>();
    default: context.error("Unknown token: {}", value.content);
    }
}

} // namespace language::parser
