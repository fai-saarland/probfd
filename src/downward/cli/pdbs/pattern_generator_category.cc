#include "downward/cli/pdbs/pattern_generator_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/pdbs/pattern_generator.h"

using namespace downward::pdbs;

using namespace language::plugins;

namespace downward::cli::pdbs {

void add_pattern_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<PatternGenerator>(
        "PatternGenerator",
        "Factory for single patterns");
}

} // namespace downward::cli::pdbs
