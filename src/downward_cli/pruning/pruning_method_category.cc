#include "downward_cli/pruning/pruning_method_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/pruning_method.h"

using namespace std;

using namespace language::plugins;

namespace downward::cli::pruning {

void add_pruning_method_category(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<PruningMethod>("PruningMethod");

    category.document_synopsis("Prune or reorder applicable operators.");
}

} // namespace downward::cli::pruning
