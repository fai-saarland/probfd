#include "downward/cli/operator_counting/constraint_generator_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/operator_counting/constraint_generator.h"

using namespace downward::operator_counting;

using namespace downward::cli::plugins;

namespace downward::cli::operator_counting {

void add_constraint_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<ConstraintGenerator>(
        "ConstraintGenerator",
        "");
}

} // namespace downward::cli::operator_counting
