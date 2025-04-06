#include "downward/cli/plugins/plugin.h"

#include "downward/pdbs/pattern_generator.h"

using namespace downward::pdbs;

using namespace downward::cli::plugins;

namespace {

class PatternCollectionGeneratorCategoryPlugin
    : public TypedCategoryPlugin<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PatternCollectionGenerator")
    {
        document_synopsis("Factory for pattern collections");
    }
} _category_plugin_collection;

class PatternGeneratorCategoryPlugin
    : public TypedCategoryPlugin<PatternGenerator> {
public:
    PatternGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PatternGenerator")
    {
        document_synopsis("Factory for single patterns");
    }
} _category_plugin_single;

} // namespace
