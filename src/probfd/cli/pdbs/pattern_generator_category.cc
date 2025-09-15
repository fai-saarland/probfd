#include "probfd/cli/pdbs/pattern_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/pdbs/pattern_generator.h"

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace probfd::cli::pdbs {

void add_pattern_generator_category(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<PatternGenerator>(
        "PPDBPatternGenerator");
    category.document_synopsis(
        "Factory for a pattern and/or the corresponding "
        "probability-aware PDB");
}

} // namespace probfd::cli::pdbs
