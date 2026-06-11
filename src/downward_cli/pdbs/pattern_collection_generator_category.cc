#include "downward_cli/pdbs/pattern_collection_generator_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/pdbs/pattern_generator.h"

using namespace downward::pdbs;

using namespace language::plugins;

namespace downward::cli::pdbs {

void add_pattern_collection_generator_category(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<PatternCollectionGenerator>(
            "PatternCollectionGenerator");
    category.document_synopsis("Factory for pattern collections");
}

} // namespace downward::cli::pdbs
