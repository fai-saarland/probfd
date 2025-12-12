#include "language/typed_ast/decorated_bool_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/context.h"

#include <any>
#include <print>

using namespace std;

namespace language::parser {

BoolLiteralNode::BoolLiteralNode(bool value)
    : value(value)
{
}

std::any BoolLiteralNode::construct(ConstructContext& context) const
{
    TraceBlock block(context, "Constructing bool value from '{}'", value);
    return value;
}

void BoolLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

} // namespace language::parser