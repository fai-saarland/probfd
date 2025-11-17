#include "probfd/cli/merge_and_shrink/shrink_strategy_bisimulation.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/shrink_strategy_bisimulation.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

#include <deque>
#include <memory>

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace {
class ShrinkBisimulationFeature
    : public SharedTypedFeature<
          ShrinkStrategy,
          ShrinkStrategyBisimulation::AtLimit,
          bool> {
public:
    ShrinkBisimulationFeature()
        : TypedFeature("pshrink_bisimulation", &ShrinkBisimulationFeature::func)
    {
        document_title("Bismulation based shrink strategy");
        document_synopsis(
            "This shrink strategy implements the algorithm described in"
            " the paper:" +
            utils::format_conference_reference(
                {"Raz Nissim", "Joerg Hoffmann", "Malte Helmert"},
                "Computing Perfect Heuristics in Polynomial Time: On "
                "Bisimulation"
                " and Merge-and-Shrink Abstractions in Optimal Planning.",
                "https://ai.dmi.unibas.ch/papers/nissim-et-al-ijcai2011.pdf",
                "Proceedings of the Twenty-Second International Joint "
                "Conference"
                " on Artificial Intelligence (IJCAI 2011)",
                "1983-1990",
                "AAAI Press",
                "2011"));

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
        const utils::Context&,
        ShrinkStrategyBisimulation::AtLimit at_limit,
        bool require_goal_distances)
    {
        return make_shared_from_arg_tuples<ShrinkStrategyBisimulation>(
            at_limit,
            require_goal_distances);
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_bisimulation_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<ShrinkStrategyBisimulation::AtLimit>(
        {{"return", "stop without refining the equivalence class further"},
         {"use_up",
          "continue refining the equivalence class until "
          "the size limit is hit"}});

    n.insert_feature_plugin<ShrinkBisimulationFeature>();
}

} // namespace probfd::cli::merge_and_shrink
