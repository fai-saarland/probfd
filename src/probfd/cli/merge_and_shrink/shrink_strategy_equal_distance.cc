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
    : public SharedTypedFeature<
          ShrinkStrategy,
          int,
          ShrinkStrategyEqualDistance::Priority> {
public:
    ShrinkStrategyEqualDistanceFeature()
        : TypedFeature(
              "shrink_equal_distance",
              &ShrinkStrategyEqualDistanceFeature::func)
    {
        document_title("distance-preserving shrink strategy");

        document_note(
            "Note",
            "The strategy first partitions all states according to their "
            "h-values. States sorted last are shrinked together until reaching "
            "max_states.");

        const auto n = add_bucket_based_shrink_options_to_feature(*this, 0);

        make_optional_argument_with_default(
            n,
            "priority",
            "low",
            "in which direction the distance based shrink priority is ordered");
    }

protected:
    static shared_ptr<ShrinkStrategy> func(
        int random_seed,
        ShrinkStrategyEqualDistance::Priority high_low)
    {
        return make_shared_from_arg_tuples<ShrinkStrategyEqualDistance>(
            random_seed,
            high_low);
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
