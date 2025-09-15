#include "probfd/cli/occupation_measures/constraint_generator_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/occupation_measures/constraint_generator.h"

using namespace probfd::occupation_measures;

using namespace downward::cli::plugins;

namespace probfd::cli::occupation_measures {

void add_constraint_generator_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<ConstraintGenerator>(
        "OMConstraintGeneratorFactory");
}

} // namespace probfd::cli::occupation_measures
