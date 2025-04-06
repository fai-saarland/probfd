#include "downward/cli/plugins/plugin.h"

#include "downward/cartesian_abstractions/subtask_generators.h"

using namespace downward::cartesian_abstractions;

using namespace downward::cli::plugins;

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
