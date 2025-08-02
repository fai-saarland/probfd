#include "downward/cli/pdbs/pattern_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/pdbs/pattern_generator.h"

using namespace downward::pdbs;

using namespace downward::cli::plugins;

namespace {
class PatternGeneratorCategoryPlugin
    : public TypedCategoryPlugin<PatternGenerator> {
public:
    PatternGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PatternGenerator")
    {
        document_synopsis("Factory for single patterns");
    }
};
}

namespace downward::cli::pdbs {

void add_pattern_generator_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<PatternGeneratorCategoryPlugin>();
}

} // namespace
