#include "downward/cli/plugins/plugin.h"

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

PruningMethodCategoryPlugin _category_plugin;

} // namespace
