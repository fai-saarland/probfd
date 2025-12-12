#include "downward/cli/merge_and_shrink/shrink_bisimulation_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/merge_and_shrink/shrink_bisimulation.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;

using namespace downward::utils;
using namespace language::plugins;

using namespace downward::merge_and_shrink;

namespace {

InternalFunctionDefinitionBase& add_shrink_strategy_bisimulation_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "shrink_bisimulation",
        &language::plugins::
            construct_shared<ShrinkStrategy, ShrinkBisimulation, bool, AtLimit>);
    f.document_title("Bismulation based shrink strategy");
    f.document_synopsis(
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

    f.document_note(
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
    f.document_note(
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

    f.make_optional_argument_with_default(
        0,
        "greedy",
        "false",
        "use greedy bisimulation");
    f.make_optional_argument_with_default(
        1,
        "at_limit",
        "return",
        "what to do when the size limit is hit");

    return f;
}

} // namespace

namespace downward::cli::merge_and_shrink {

void add_shrink_bisimulation_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<AtLimit>(
        {{"return", "stop without refining the equivalence class further"},
         {"use_up",
          "continue refining the equivalence class until "
          "the size limit is hit"}});

    add_shrink_strategy_bisimulation_to_namespace(n);
}

} // namespace downward::cli::merge_and_shrink
