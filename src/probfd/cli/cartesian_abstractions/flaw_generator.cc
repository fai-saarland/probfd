#include "downward/cli/plugins/plugin.h"

#include "probfd/cartesian_abstractions/flaw_generator.h"

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace {

class FlawGeneratorFactoryCategoryPlugin
    : public TypedCategoryPlugin<FlawGeneratorFactory> {
public:
    FlawGeneratorFactoryCategoryPlugin()
        : TypedCategoryPlugin("FlawGeneratorFactory")
    {
        document_synopsis("Factory for flaw generation algorithms used in the "
                          "cartesian abstraction "
                          "refinement loop");
    }
} _category_plugin;

} // namespace
