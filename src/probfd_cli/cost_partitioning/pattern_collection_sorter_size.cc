#include "probfd_cli/cost_partitioning/pattern_collection_sorter_size.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/cost_partitioning/pattern_collection_sorter_size_factory.h"

#include "downward_cli/utils/rng_options.h"

using namespace downward;
using namespace probfd;
using namespace probfd::cost_partitioning;

using namespace language;
using namespace language::plugins;

namespace {

class PatternCollectionSorterSizeAscendingFactoryFeature
    : public SharedTypedFeature<PatternCollectionSorterFactory> {
public:
    PatternCollectionSorterSizeAscendingFactoryFeature()
        : TypedFeature("pattern_collection_sorter_size_ascending")
    {
        document_title("Ascending Size Pattern Collection Sorter");
    }

    std::shared_ptr<PatternCollectionSorterFactory>
    create_component(const Options&, const Context&) const override
    {
        return create_pattern_collection_sorter_size_ascending_factory();
    }
};

class PatternCollectionSorterSizeDescendingFactoryFeature
    : public SharedTypedFeature<PatternCollectionSorterFactory> {
public:
    PatternCollectionSorterSizeDescendingFactoryFeature()
        : TypedFeature("pattern_collection_sorter_size_descending")
    {
        document_title("Descending Size Pattern Collection Sorter");
    }

    std::shared_ptr<PatternCollectionSorterFactory>
    create_component(const Options&, const Context&) const override
    {
        return create_pattern_collection_sorter_size_descending_factory();
    }
};

} // namespace

namespace probfd::cli::cost_partitioning {

void add_pattern_collection_sorter_size_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<
        PatternCollectionSorterSizeAscendingFactoryFeature>();
    raw_registry.insert_feature_plugin<
        PatternCollectionSorterSizeDescendingFactoryFeature>();
}

} // namespace probfd::cli::cost_partitioning
