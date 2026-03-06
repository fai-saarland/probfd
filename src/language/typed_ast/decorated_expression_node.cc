#include "language/typed_ast/decorated_expression_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/context.h"

#include <any>
#include <functional>

using namespace std;

namespace language::typed_ast {

std::any construct(const DecoratedExpressionNode& node)
{
    ConstructContext context;
    TraceBlock block(context, "Constructing parsed object");
    return node.construct(context);
}

} // namespace language::parser