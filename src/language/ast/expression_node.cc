#include "language/ast/expression_node.h"

#include "language/ast/variable_environment.h"

#include "language/typed_ast/symbol_node.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::cli::parser {

DecoratedASTNodePtr
ASTNode::static_analysis(const plugins::Registry& registry) const
{
    utils::Context context;
    VariableEnvironment env(registry);
    utils::TraceBlock block(context, "Start semantic analysis");
    return static_analysis(context, env).ast_node;
}

} // namespace downward::cli::parser
