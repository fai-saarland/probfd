#include "downward/cli/pruning/pruning_method_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/pruning_method.h"

using namespace std;

namespace downward::cli::pruning {

void add_pruning_method_category(plugins::RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<PruningMethod>("PruningMethod");

    category.document_synopsis("Prune or reorder applicable operators.");
}

} // namespace downward::cli::pruning
