#include "language/typed_ast/decorated_string_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "downward/utils/logging.h"

#include <ranges>

using namespace std;

namespace downward::cli::parser {

StringLiteralNode::StringLiteralNode(const string& value)
    : value(value)
{
}

std::any StringLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing string value from '{}'",
        value);

    return value;
}

void StringLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{:>{}?}", value, indent + value.size());
}

} // namespace downward::cli::parser