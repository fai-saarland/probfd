#include "downward/cli/pdbs/pattern_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pdbs/pattern_generator.h"

using namespace downward::pdbs;

using namespace downward::cli::plugins;

namespace downward::cli::pdbs {

void add_pattern_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<PatternGenerator>(
        "PatternGenerator",
        "Factory for single patterns");
}

} // namespace downward::cli::pdbs
