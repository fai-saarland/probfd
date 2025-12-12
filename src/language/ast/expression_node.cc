#include "language/ast/expression_node.h"

#include "language/ast/variable_environment.h"

#include "language/typed_ast/symbol_node.h"

#include "language/context.h"

using namespace std;

namespace language::parser {

DecoratedASTNodePtr
ASTNode::static_analysis(const plugins::Registry& registry) const
{
    Context context;
    VariableEnvironment env(registry);
    TraceBlock block(context, "Start semantic analysis");
    return static_analysis(context, env).ast_node;
}

} // namespace language::parser
