#include "language/ast/type_literal_node.h"

#include "language/typed_ast/type_registry.h"

#include "language/context.h"

using namespace std;

namespace language::parser {

TypeLiteralNode::TypeLiteralNode(const Token& value)
    : value(value)
{
}

const typed_ast::Type& TypeLiteralNode::get_type(
    Context& context,
    const typed_ast::GlobalEnvironment&,
    typed_ast::TypeRegistry& type_registry) const
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
