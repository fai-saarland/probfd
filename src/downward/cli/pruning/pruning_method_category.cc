#include "downward/cli/pruning/pruning_method_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pruning_method.h"

using namespace std;

namespace downward::cli::pruning {

void add_pruning_method_category(plugins::Registry& registry)
{
    plugins::Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<PruningMethod>(
        "PruningMethod",
        "Prune or reorder applicable operators.");
}

} // namespace downward::cli::pruning
