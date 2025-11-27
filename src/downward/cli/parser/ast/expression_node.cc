#include "downward/cli/parser/ast/expression_node.h"

#include "downward/cli/parser/ast/variable_environment.h"

#include "downward/cli/parser/decorated_abstract_syntax_tree.h"

#include "downward/cli/parser/lexical_analyzer.h"
#include "downward/cli/parser/syntax_analyzer.h"
#include "downward/cli/parser/token_stream.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/types.h"

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
