#include "language/typed_ast/decorated_float_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/context.h"

#include <any>
#include <print>

using namespace std;

namespace language::typed_ast {

FloatLiteralNode::FloatLiteralNode(double value)
    : value(value)
{
}

std::any FloatLiteralNode::construct(ConstructContext& context) const
{
    TraceBlock block(context, "Constructing float value from '{}'", value);
    return value;
}

} // namespace language::parser