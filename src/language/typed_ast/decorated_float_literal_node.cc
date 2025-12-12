#include "language/typed_ast/decorated_float_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/context.h"

#include <any>
#include <print>

using namespace std;

namespace language::parser {

FloatLiteralNode::FloatLiteralNode(double value)
    : value(value)
{
}

std::any FloatLiteralNode::construct(ConstructContext& context) const
{
    TraceBlock block(context, "Constructing float value from '{}'", value);
    return value;
}

void FloatLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

} // namespace language::parser