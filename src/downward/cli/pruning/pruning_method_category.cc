#include "downward/cli/pruning/pruning_method_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/pruning_method.h"

using namespace std;

using namespace downward::cli;

namespace {
class PruningMethodCategoryPlugin
    : public plugins::TypedCategoryPlugin<downward::PruningMethod> {
public:
    PruningMethodCategoryPlugin()
        : TypedCategoryPlugin("PruningMethod")
    {
        document_synopsis("Prune or reorder applicable operators.");
    }
};
}

namespace downward::cli::pruning {

void add_pruning_method_category(plugins::RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<PruningMethodCategoryPlugin>();
}

} // namespace
