#include "downward/cli/plugins/plugin.h"

#include "probfd/occupation_measures/constraint_generator.h"

using namespace probfd::occupation_measures;

using namespace downward::cli::plugins;

namespace {

class ConstraintGeneratorCategoryPlugin
    : public TypedCategoryPlugin<ConstraintGenerator> {
public:
    ConstraintGeneratorCategoryPlugin()
        : TypedCategoryPlugin("OMConstraintGenerator")
    {
    }
} _category_plugin;

} // namespace
