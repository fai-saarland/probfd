#include "language/typed_ast/convert_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/plugins/types.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::cli::parser {

ConvertNode::ConvertNode(
    DecoratedASTNodePtr value,
    const plugins::Type& from_type,
    const plugins::Type& to_type)
    : value(move(value))
    , from_type(from_type)
    , to_type(to_type)
{
}

void ConvertNode::remove_variable_usages()
{
    value->remove_variable_usages();
}

std::any ConvertNode::construct(ConstructContext& context) const
{
    utils::TraceBlock cblock(
        context,
        "Constructing value that requires conversion");

    const std::any constructed_value = [&] {
        utils::TraceBlock block(
            context,
            "Constructing value of type '{}'",
            from_type.name());
        return value->construct(context);
    }();

    std::any converted_value = [&] {
        utils::TraceBlock block(
            context,
            "Converting constructed value from '{}' to '{}'",
            from_type.name(),
            to_type.name());
        return plugins::convert(constructed_value, from_type, to_type, context);
    }();

    return converted_value;
}

void ConvertNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    value->print(out, indent, print_default_args);
}

} // namespace downward::cli::parser