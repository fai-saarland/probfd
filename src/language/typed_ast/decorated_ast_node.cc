#include "language/typed_ast/decorated_ast_node.h"

#include "language/typed_ast/construct_context.h"
#include "language/typed_ast/variable_definition.h"

#include "downward/utils/logging.h"

#include <any>
#include <functional>

using namespace std;

namespace downward::cli::parser {

std::vector<VariableDefinition> DecoratedASTNode::prune_unused_definitions()
{
    std::vector<VariableDefinition> defs;
    prune_unused_definitions(defs);
    return defs;
}

std::any DecoratedASTNode::construct() const
{
    ConstructContext context;
    utils::TraceBlock block(context, "Constructing parsed object");
    return construct(context);
}

} // namespace downward::cli::parser