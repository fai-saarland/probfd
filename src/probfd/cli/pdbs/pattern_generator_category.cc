#include "probfd/cli/pdbs/pattern_generator_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/pdbs/pattern_generator.h"

using namespace probfd::pdbs;

using namespace language::plugins;

namespace probfd::cli::pdbs {

void add_pattern_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<PatternGenerator>(
        "PPDBPatternGenerator",
        "Factory for a pattern and/or the corresponding "
        "probability-aware PDB");
}

} // namespace probfd::cli::pdbs
