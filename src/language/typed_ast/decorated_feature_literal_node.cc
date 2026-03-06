#include "language/typed_ast/decorated_feature_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/ast/internal_function_definition.h"

#include "language/context.h"

#include <any>
#include <ranges>

using namespace std;

namespace language::typed_ast {

FeatureLiteralNode::FeatureLiteralNode(AnyFunctionType callee)
    : callee(callee)
{
}

std::any FeatureLiteralNode::construct(ConstructContext& context) const
{
    TraceBlock block(context, "Constructing feature");
    return callee;
}

} // namespace language::parser