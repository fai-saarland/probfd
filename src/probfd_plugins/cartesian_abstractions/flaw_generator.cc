#include "probfd/cartesian_abstractions/flaw_generator.h"

#include "downward/plugins/plugin.h"

using namespace probfd::cartesian_abstractions;

namespace {

static class FlawGeneratorFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<FlawGeneratorFactory> {
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
