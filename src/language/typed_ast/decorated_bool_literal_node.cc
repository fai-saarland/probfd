#include "language/typed_ast/decorated_bool_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/context.h"

#include <any>
#include <print>

using namespace std;

namespace language::typed_ast {

BoolLiteralNode::BoolLiteralNode(bool value)
    : value(value)
{
}

std::any BoolLiteralNode::construct(ConstructContext& context) const
{
    TraceBlock block(context, "Constructing bool value from '{}'", value);
    return value;
}

} // namespace language::parser