#include "downward/cli/plugins/plugin.h"

#include "downward/merge_and_shrink/shrink_bisimulation.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;

using namespace downward::utils;
using namespace downward::cli::plugins;

using namespace downward::merge_and_shrink;

namespace {

class ShrinkBisimulationFeature
    : public TypedFeature<ShrinkStrategy, ShrinkBisimulation> {
public:
    ShrinkBisimulationFeature()
        : TypedFeature("shrink_bisimulation")
    {
        document_title("Bismulation based shrink strategy");
        document_synopsis(
            "This shrink strategy implements the algorithm described in"
            " the paper:" +
            format_conference_reference(
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

        add_option<bool>("greedy", "use greedy bisimulation", "false");
        add_option<AtLimit>(
            "at_limit",
            "what to do when the size limit is hit",
            "return");

        document_note(
            "shrink_bisimulation(greedy=true)",
            "Combine this with the merge-and-shrink options "
            "max_states=infinity "
            "and threshold_before_merge=1 and with the linear merge strategy "
            "reverse_level to obtain the variant 'greedy bisimulation without "
            "size "
            "limit', called M&S-gop in the IJCAI 2011 paper. "
            "When we last ran experiments on interaction of shrink strategies "
            "with label reduction, this strategy performed best when used with "
            "label reduction before shrinking (and no label reduction before "
            "merging).");
        document_note(
            "shrink_bisimulation(greedy=false)",
            "Combine this with the merge-and-shrink option max_states=N (where "
            "N "
            "is a numerical parameter for which sensible values include 1000, "
            "10000, 50000, 100000 and 200000) and with the linear merge "
            "strategy "
            "reverse_level to obtain the variant 'exact bisimulation with a "
            "size "
            "limit', called DFP-bop in the IJCAI 2011 paper. "
            "When we last ran experiments on interaction of shrink strategies "
            "with label reduction, this strategy performed best when used with "
            "label reduction before shrinking (and no label reduction before "
            "merging).");
    }

    virtual shared_ptr<ShrinkBisimulation>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared<ShrinkBisimulation>(
            opts.get<bool>("greedy"),
            opts.get<AtLimit>("at_limit"));
    }
};

FeaturePlugin<ShrinkBisimulationFeature> _plugin;

TypedEnumPlugin<AtLimit> _enum_plugin(
    {{"return", "stop without refining the equivalence class further"},
     {"use_up",
      "continue refining the equivalence class until "
      "the size limit is hit"}});

}
