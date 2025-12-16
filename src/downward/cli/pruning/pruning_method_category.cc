#include "downward/cli/pruning/pruning_method_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/pruning_method.h"

using namespace std;

namespace downward::cli::pruning {

void add_pruning_method_category(language::plugins::Registry& registry)
{
    language::plugins::Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<PruningMethod>(
        "PruningMethod",
        "Prune or reorder applicable operators.");
}

} // namespace downward::cli::pruning
