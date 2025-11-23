#include "downward/cli/pdbs/pattern_collection_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pdbs/pattern_generator.h"

using namespace downward::pdbs;

using namespace downward::cli::plugins;

namespace downward::cli::pdbs {

void add_pattern_collection_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<PatternCollectionGenerator>(
        "PatternCollectionGenerator",
        "Factory for pattern collections");
}

} // namespace downward::cli::pdbs
