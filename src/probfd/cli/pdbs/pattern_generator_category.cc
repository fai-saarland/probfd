#include "probfd/cli/pdbs/pattern_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/pdbs/pattern_generator.h"

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {
class PatternGeneratorCategoryPlugin
    : public TypedCategoryPlugin<PatternGenerator> {
public:
    PatternGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PPDBPatternGenerator")
    {
        document_synopsis(
            "Factory for a pattern and/or the corresponding "
            "probability-aware PDB");
    }
};
}

namespace probfd::cli::pdbs {

void add_pattern_generator_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<PatternGeneratorCategoryPlugin>();
}

} // namespace
