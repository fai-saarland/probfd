#include "probfd/merge_and_shrink/shrink_strategy_probabilistic_bisimulation.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace {

class ShrinkProbabilisticBisimulationFeature
    : public TypedFeature<
          ShrinkStrategy,
          ShrinkStrategyProbabilisticBisimulation> {
public:
    ShrinkProbabilisticBisimulationFeature()
        : TypedFeature("pshrink_bisimulation")
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

        add_option<ShrinkStrategyProbabilisticBisimulation::AtLimit>(
            "at_limit",
            "what to do when the size limit is hit",
            "return");

        add_option<bool>(
            "require_goal_distances",
            "whether goal distances are required",
            "true");
    }

protected:
    shared_ptr<ShrinkStrategyProbabilisticBisimulation>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<
            ShrinkStrategyProbabilisticBisimulation>(
            options.get<ShrinkStrategyProbabilisticBisimulation::AtLimit>(
                "at_limit"),
            options.get<bool>("require_goal_distances"));
    }
};

FeaturePlugin<ShrinkProbabilisticBisimulationFeature> _plugin;

TypedEnumPlugin<ShrinkStrategyProbabilisticBisimulation::AtLimit> _enum_plugin(
    {{"return", "stop without refining the equivalence class further"},
     {"use_up",
      "continue refining the equivalence class until "
      "the size limit is hit"}});

} // namespace
