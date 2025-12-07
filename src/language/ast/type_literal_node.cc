#include "language/ast/type_literal_node.h"

#include "language/plugins/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/system.h"

using namespace std;

namespace downward::cli::parser {

TypeLiteralNode::TypeLiteralNode(const Token& value)
    : value(value)
{
}

const plugins::Type& TypeLiteralNode::get_type(
    utils::Context& context,
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

} // namespace downward::cli::parser
