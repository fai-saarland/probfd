#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace occupation_measures {

static class ConstraintGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<ConstraintGenerator> {
public:
    ConstraintGeneratorCategoryPlugin()
        : TypedCategoryPlugin("OMConstraintGenerator")
    {
    }
} _category_plugin;

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd