#include "language/typed_ast/convert_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/typed_ast/types.h"

#include "language/context.h"

using namespace std;

namespace language::typed_ast {

ConvertNode::ConvertNode(
    std::unique_ptr<DecoratedExpressionNode> value,
    const Type& from_type,
    const Type& to_type)
    : value(move(value))
    , from_type(from_type)
    , to_type(to_type)
{
}

std::any ConvertNode::construct(ConstructContext& context) const
{
    TraceBlock cblock(context, "Constructing value that requires conversion");

    const std::any constructed_value = [&] {
        TraceBlock block(
            context,
            "Constructing value of type '{}'",
            from_type.name());
        return value->construct(context);
    }();

    std::any converted_value = [&] {
        TraceBlock block(
            context,
            "Converting constructed value from '{}' to '{}'",
            from_type.name(),
            to_type.name());
        return convert(constructed_value, from_type, to_type, context);
    }();

    return converted_value;
}

} // namespace language::parser