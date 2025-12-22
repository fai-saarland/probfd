#include "language/typed_ast/decorated_ast_node.h"

#include "language/typed_ast/construct_context.h"
#include "language/typed_ast/variable_declaration.h"

#include "language/context.h"

#include <any>
#include <functional>

using namespace std;

namespace language::parser {

std::vector<std::unique_ptr<VariableDeclaration>>
DecoratedASTNode::prune_unused_definitions()
{
    std::vector<std::unique_ptr<VariableDeclaration>> defs;
    prune_unused_definitions(defs);
    return defs;
}

std::any DecoratedASTNode::construct() const
{
    ConstructContext context;
    TraceBlock block(context, "Constructing parsed object");
    return construct(context);
}

} // namespace language::parser