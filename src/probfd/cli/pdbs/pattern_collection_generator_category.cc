#include "probfd/cli/pdbs/pattern_collection_generator_category.h"

#include "language/plugins/registry.h"

#include "probfd/pdbs/pattern_collection_generator.h"

using namespace probfd::pdbs;

using namespace language::plugins;

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<PatternCollectionGenerator>(
        "PPDBPatternCollectionGenerator",
        "Factory for pattern collections and/or "
        "corresponding probability-aware "
        "PDBs");
}

} // namespace probfd::cli::pdbs
