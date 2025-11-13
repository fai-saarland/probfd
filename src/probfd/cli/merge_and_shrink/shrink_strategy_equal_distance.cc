#include "probfd/cli/merge_and_shrink/shrink_strategy_equal_distance.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
class ShrinkStrategyEqualDistanceFeature
    : public SharedTypedFeature<ShrinkStrategy> {
public:
    ShrinkStrategyEqualDistanceFeature()
        : TypedFeature("shrink_equal_distance")
    {
        document_title("distance-preserving shrink strategy");

        add_bucket_based_shrink_options_to_feature(*this);

        add_optional_argument_with_default<
            ShrinkStrategyEqualDistance::Priority>(
            "priority",
            "low",
            "in which direction the distance based shrink priority is ordered");

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

void add_shrink_strategy_equal_distance_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<ShrinkStrategyEqualDistance::Priority>(
        {{"high", "prefer shrinking states with high value"},
         {"low", "prefer shrinking states with low value"}});

    n.insert_feature_plugin<ShrinkStrategyEqualDistanceFeature>();
}

} // namespace probfd::cli::merge_and_shrink
