#include "downward_plugins/plugins/plugin.h"

#include "probfd/occupation_measures/constraint_generator.h"

using namespace probfd::occupation_measures;

using namespace downward_plugins::plugins;

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
