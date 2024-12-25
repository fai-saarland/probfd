#include "downward/cli/plugins/plugin.h"

#include "downward/pruning_method.h"

using namespace std;

namespace downward::cli {

class PruningMethodCategoryPlugin
    : public plugins::TypedCategoryPlugin<PruningMethod> {
public:
    PruningMethodCategoryPlugin()
        : TypedCategoryPlugin("PruningMethod")
    {
        document_synopsis("Prune or reorder applicable operators.");
    }
} _category_plugin;

} // namespace downward::cli
