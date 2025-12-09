#include "language/typed_ast/decorated_list_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/plugins/types.h"

#include "downward/utils/logging.h"

#include <functional>
#include <ranges>

using namespace std;

namespace downward::cli::parser {

DecoratedListNode::DecoratedListNode(vector<DecoratedASTNodePtr>&& elements)
    : elements(move(elements))
{
}

void DecoratedListNode::remove_variable_usages()
{
    for (const auto& el : elements) { el->remove_variable_usages(); }
}

std::any DecoratedListNode::construct(ConstructContext& context) const
{
    utils::TraceBlock lblock(context, "Constructing list");
    vector<std::any> result;
    int i = 0;
    for (const DecoratedASTNodePtr& element : elements) {
        utils::TraceBlock block(context, "Constructing element {}", i++);
        result.push_back(element->construct(context));
    }
    return result;
}

void DecoratedListNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(out, "{:>{}}", "[", indent + 1);

    if (!elements.empty()) {
        {
            const auto& el = elements.front();
            el->print(out, 0, print_default_args);
        }

        for (const auto& el : elements | std::views::drop(1)) {
            std::print(out, ", ");
            el->print(out, 0, print_default_args);
        }
    }

    std::print(out, "]");
}

} // namespace downward::cli::parser