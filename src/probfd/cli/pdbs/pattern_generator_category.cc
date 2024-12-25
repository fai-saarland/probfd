#include "downward/cli/plugins/plugin.h"

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
        document_synopsis("Factory for a pattern and/or the corresponding "
                          "probability-aware PDB");
    }
} _category_plugin;

} // namespace
