#include "downward/cli/plugins/plugin.h"

#include "probfd/occupation_measures/constraint_generator.h"

using namespace probfd::occupation_measures;

using namespace downward::cli::plugins;

namespace {

class ConstraintGeneratorFactoryCategoryPlugin
    : public TypedCategoryPlugin<ConstraintGenerator> {
public:
    ConstraintGeneratorFactoryCategoryPlugin()
        : TypedCategoryPlugin("OMConstraintGeneratorFactory")
    {
    }
} _category_plugin;

} // namespace
