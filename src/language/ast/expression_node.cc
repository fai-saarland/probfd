#include "language/ast/expression_node.h"

#include "language/ast/variable_environment.h"

#include "language/typed_ast/decorated_ast_node.h"

#include "language/context.h"

using namespace std;

namespace language::parser {

std::unique_ptr<DecoratedASTNode>
ASTNode::static_analysis(const plugins::Registry& registry) const
{
    Context context;
    VariableEnvironment env(registry, context);
    TraceBlock block(context, "Start semantic analysis");
    return static_analysis(context, env).ast_node;
}

} // namespace language::parser
