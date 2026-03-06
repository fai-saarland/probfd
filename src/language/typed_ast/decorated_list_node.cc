#include "language/typed_ast/decorated_list_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/context.h"

#include <ranges>

using namespace std;

namespace language::typed_ast {

DecoratedListNode::DecoratedListNode(
    vector<std::unique_ptr<DecoratedExpressionNode>>&& elements)
    : elements(move(elements))
{
}

std::any DecoratedListNode::construct(ConstructContext& context) const
{
    TraceBlock lblock(context, "Constructing list");
    vector<std::any> result;
    int i = 0;
    for (const std::unique_ptr<DecoratedExpressionNode>& element : elements) {
        TraceBlock block(context, "Constructing element {}", i++);
        result.push_back(element->construct(context));
    }
    return result;
}

} // namespace language::typed_ast