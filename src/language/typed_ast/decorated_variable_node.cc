#include "language/typed_ast/decorated_variable_node.h"

#include "language/typed_ast/construct_context.h"
#include "language/typed_ast/variable_definition.h"

#include "language/plugins/types.h"

#include "downward/utils/logging.h"

#include <cassert>

using namespace std;

namespace downward::cli::parser {

VariableNode::VariableNode(VariableDeclaration& declaration)
    : declaration(&declaration)
{
}

void VariableNode::remove_variable_usages()
{
    const auto it = std::ranges::find(declaration->usages, this);
    assert(it != declaration->usages.end());
    declaration->usages.erase(it);
}

std::any VariableNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Looking up variable '{}'",
        declaration->variable_name);

    if (!context.has_variable(declaration->variable_name)) {
        context.error(
            "Variable '{}' is not defined.",
            declaration->variable_name);
    }

    return context.get_variable(declaration->variable_name);
}

void VariableNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(
        out,
        "{:>{}}",
        declaration->variable_name,
        indent + declaration->variable_name.size());
}

} // namespace downward::cli::parser