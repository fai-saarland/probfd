#include "probfd/cli/pdbs/pattern_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/pdbs/pattern_generator.h"

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace probfd::cli::pdbs {

void add_pattern_generator_category(Registry& raw_registry)
{
    raw_registry.insert_shared_category_plugin<PatternGenerator>(
        "PPDBPatternGenerator",
        "Factory for a pattern and/or the corresponding "
        "probability-aware PDB");
}

} // namespace probfd::cli::pdbs
