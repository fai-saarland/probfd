#include "probfd/cli/merge_and_shrink/shrink_strategy_equal_distance.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/cli/merge_and_shrink/shrink_strategy_bucket_based_options.h"

#include "probfd/merge_and_shrink/shrink_strategy_equal_distance.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {
class ShrinkStrategyEqualDistanceFeature : public TypedFeature<ShrinkStrategy> {
public:
    ShrinkStrategyEqualDistanceFeature()
        : TypedFeature("shrink_equal_distance")
    {
        document_title("distance-preserving shrink strategy");

        add_bucket_based_shrink_options_to_feature(*this);

        add_option<ShrinkStrategyEqualDistance::Priority>(
            "priority",
            "in which direction the distance based shrink priority is ordered",
            "low");

        document_note(
            "Note",
            "The strategy first partitions all states according to their "
            "h-values. States sorted last are shrinked together until reaching "
            "max_states.");
    }

protected:
    shared_ptr<ShrinkStrategy>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<ShrinkStrategyEqualDistance>(
            get_bucket_based_shrink_args_from_options(options),
            options.get<ShrinkStrategyEqualDistance::Priority>("priority"));
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_equal_distance_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<ShrinkStrategyEqualDistanceFeature>();

    raw_registry.insert_enum_plugin<ShrinkStrategyEqualDistance::Priority>(
        {{"high", "prefer shrinking states with high value"},
         {"low", "prefer shrinking states with low value"}});
}

} // namespace probfd::cli::merge_and_shrink
