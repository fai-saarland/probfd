#include "language/ast/expression_node.h"

#include "language/typed_ast/variable_environment.h"

#include "language/typed_ast/type_registry.h"

#include "language/typed_ast/decorated_expression_node.h"

#include "language/context.h"

using namespace std;

namespace language::parser {

std::unique_ptr<typed_ast::DecoratedExpressionNode>
static_analysis(const ExpressionNode& node, CompilationContext& registry)
{
    Context context;
    typed_ast::TypeRegistry type_registry;
    typed_ast::GlobalEnvironment env(registry, context, type_registry);
    typed_ast::LocalEnvironment local_env;
    TraceBlock block(context, "Start semantic analysis");
    return node.static_analysis(context, env, local_env, type_registry)
        .ast_node;
}

} // namespace language::parser
