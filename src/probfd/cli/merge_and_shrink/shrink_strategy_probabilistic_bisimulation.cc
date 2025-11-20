#include "probfd/cli/merge_and_shrink/shrink_strategy_probabilistic_bisimulation.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/shrink_strategy_probabilistic_bisimulation.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace {
class ShrinkProbabilisticBisimulationFeature
    : public SharedTypedFeature<
          ShrinkStrategy,
          ShrinkStrategyProbabilisticBisimulation::AtLimit,
          bool> {
public:
    ShrinkProbabilisticBisimulationFeature()
        : TypedFeature(
              "pshrink_probabilistic_bisimulation",
              &ShrinkProbabilisticBisimulationFeature::func)
    {
        document_title("Probabilistic Bisimulation-based shrink strategy");
        document_synopsis(
            "Computes a probabilistic bisimulation over the induced "
            "probabilistic transition system and emits the corresponding "
            "abstraction mapping. This strategy is not exact.");

        make_optional_argument_with_default(
            0,
            "at_limit",
            "return",
            "what to do when the size limit is hit");

        make_optional_argument_with_default(
            1,
            "require_goal_distances",
            "true",
            "whether goal distances are required");
    }

protected:
    static shared_ptr<ShrinkStrategy> func(
        ShrinkStrategyProbabilisticBisimulation::AtLimit at_limit,
        bool require_goal_distance)
    {
        return make_shared_from_arg_tuples<
            ShrinkStrategyProbabilisticBisimulation>(
            at_limit,
            require_goal_distance);
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_probabilistic_bisimulation_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<ShrinkStrategyProbabilisticBisimulation::AtLimit>(
        {{"return", "stop without refining the equivalence class further"},
         {"use_up",
          "continue refining the equivalence class until "
          "the size limit is hit"}});

    n.insert_feature_plugin<ShrinkProbabilisticBisimulationFeature>();
}

} // namespace probfd::cli::merge_and_shrink
