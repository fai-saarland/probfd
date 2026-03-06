#include "language/typed_ast/decorated_string_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/context.h"

#include <print>

using namespace std;

namespace language::typed_ast {

StringLiteralNode::StringLiteralNode(const string& value)
    : value(value)
{
}

std::any StringLiteralNode::construct(ConstructContext& context) const
{
    TraceBlock block(context, "Constructing string value from '{}'", value);

    return value;
}

} // namespace language::parser