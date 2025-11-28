#include "downward/cli/parser/ast/type_literal_node.h"

#include "downward/cli/plugins/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/system.h"

using namespace std;

namespace downward::cli::parser {

TypeLiteralNode::TypeLiteralNode(const Token& value)
    : value(value)
{
}

const plugins::Type&
TypeLiteralNode::get_type(plugins::TypeRegistry& type_registry) const
{
    switch (value.type) {
    case TokenType::TYPE_BOOL: return type_registry.get_type<bool>();
    case TokenType::TYPE_INTEGER: return type_registry.get_type<int>();
    case TokenType::TYPE_FLOAT: return type_registry.get_type<float>();
    case TokenType::TYPE_STRING: return type_registry.get_type<std::string>();
    case TokenType::IDENTIFIER:
        throw new utils::CriticalError(
            "Type literals for user-defined types not implemented.");
    default: throw new utils::CriticalError("Unknown token: {}", value.content);
    }
}

} // namespace downward::cli::parser
