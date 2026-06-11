#include "probfd_cli/pdbs/pattern_collection_generator_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/pdbs/pattern_collection_generator.h"

using namespace probfd::pdbs;

using namespace language::plugins;

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_category(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<PatternCollectionGenerator>(
            "PPDBPatternCollectionGenerator");
    category.document_synopsis(
        "Factory for pattern collections and/or "
        "corresponding probability-aware "
        "PDBs");
}

} // namespace probfd::cli::pdbs
