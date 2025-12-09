#include "language/typed_ast/decorated_int_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "downward/utils/logging.h"

#include <ranges>

using namespace std;

namespace downward::cli::parser {

IntLiteralNode::IntLiteralNode(int value)
    : value(value)
{
}

std::any IntLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(context, "Constructing int value from '{}'", value);
    return value;
}

void IntLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

} // namespace downward::cli::parser