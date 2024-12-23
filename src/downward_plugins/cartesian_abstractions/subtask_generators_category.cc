#include "downward_plugins/plugins/plugin.h"

#include "downward/cartesian_abstractions/subtask_generators.h"

using namespace cartesian_abstractions;

using namespace downward_plugins::plugins;

namespace {

class SubtaskGeneratorCategoryPlugin
    : public TypedCategoryPlugin<SubtaskGenerator> {
public:
    SubtaskGeneratorCategoryPlugin()
        : TypedCategoryPlugin("SubtaskGenerator")
    {
        document_synopsis("Subtask generator (used by the CEGAR heuristic).");
    }
} _category_plugin;

} // namespace
