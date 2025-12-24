#include "language/ast/expression_node.h"

#include "language/typed_ast/variable_environment.h"

#include "language/plugins/type_registry.h"

#include "language/typed_ast/decorated_ast_node.h"

#include "language/context.h"

using namespace std;

namespace language::parser {

std::unique_ptr<DecoratedASTNode>
ASTNode::static_analysis(const plugins::Registry& registry) const
{
    Context context;
    plugins::TypeRegistry& type_registry = *plugins::TypeRegistry::instance();
    VariableEnvironment env(registry, context, type_registry);
    TraceBlock block(context, "Start semantic analysis");
    return static_analysis(context, env, type_registry).ast_node;
}

} // namespace language::parser
