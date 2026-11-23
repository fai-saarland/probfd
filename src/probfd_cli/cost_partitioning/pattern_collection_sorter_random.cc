#include "probfd_cli/cost_partitioning/pattern_collection_sorter_random.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/cost_partitioning/pattern_collection_sorter_random_factory.h"
#include "probfd/cost_partitioning/pattern_collection_sorter_factory.h"

#include "downward_cli/utils/rng_options.h"

using namespace downward;
using namespace probfd;
using namespace probfd::cost_partitioning;

using namespace language;
using namespace language::plugins;

namespace {

class PatternCollectionSorterRandomFactoryFeature
    : public SharedTypedFeature<PatternCollectionSorterFactory> {
public:
    PatternCollectionSorterRandomFactoryFeature()
        : TypedFeature("pattern_collection_sorter_random")
    {
        document_title("Random Pattern Collection Sorter");
        downward::cli::utils::add_rng_options_to_feature(*this);
    }

    std::shared_ptr<PatternCollectionSorterFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<
            PatternCollectionSorterRandomFactory>(
            downward::cli::utils::get_rng_arguments_from_options(
                context,
                opts));
    }
};
} // namespace

namespace probfd::cli::cost_partitioning {

void add_pattern_collection_sorter_random_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<PatternCollectionSorterRandomFactoryFeature>();
}

} // namespace probfd::cli::heuristics
