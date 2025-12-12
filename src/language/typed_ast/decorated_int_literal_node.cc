#include "language/typed_ast/decorated_int_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/context.h"

#include <print>

using namespace std;

namespace language::parser {

IntLiteralNode::IntLiteralNode(int value)
    : value(value)
{
}

std::any IntLiteralNode::construct(ConstructContext& context) const
{
    TraceBlock block(context, "Constructing int value from '{}'", value);
    return value;
}

void IntLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

} // namespace language::parser