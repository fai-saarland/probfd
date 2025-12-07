#include "language/ast/expression_node.h"

#include "language/ast/variable_environment.h"

#include "language/decorated_abstract_syntax_tree.h"

#include "language/lexical_analyzer.h"
#include "language/syntax_analyzer.h"
#include "language/token_stream.h"

#include "language/plugins/plugin.h"
#include "language/plugins/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/strings.h"

#include <cassert>
#include <unordered_map>
#include <vector>

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
