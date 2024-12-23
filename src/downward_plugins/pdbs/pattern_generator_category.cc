#include "downward_plugins/plugins/plugin.h"

#include "downward/pdbs/pattern_generator.h"

using namespace pdbs;

using namespace downward_plugins::plugins;

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
