#include "downward/cli/operator_counting/constraint_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/operator_counting/constraint_generator.h"

using namespace downward::operator_counting;

using namespace downward::cli::plugins;

namespace downward::cli::operator_counting {

void add_constraint_generator_category(Registry& raw_registry)
{
    raw_registry.insert_shared_category_plugin<ConstraintGenerator>(
        "ConstraintGenerator",
        "");
}

} // namespace downward::cli::operator_counting
