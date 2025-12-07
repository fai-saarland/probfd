#include "probfd/cli/merge_and_shrink/shrink_strategy_bisimulation.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

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

InternalFunctionDefinitionBase& add_shrink_strategy_bisimulation_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pshrink_bisimulation",
        &downward::cli::plugins::construct_shared<
            ShrinkStrategy,
            ShrinkStrategyBisimulation,
            ShrinkStrategyBisimulation::AtLimit,
            bool>);

    f.document_title("Bismulation based shrink strategy");
    f.document_synopsis(
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

    f.make_optional_argument_with_default(
        0,
        "at_limit",
        "return",
        "what to do when the size limit is hit");

    f.make_optional_argument_with_default(
        1,
        "require_goal_distances",
        "true",
        "whether goal distances are required");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_bisimulation_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<ShrinkStrategyBisimulation::AtLimit>(
        {{"return", "stop without refining the equivalence class further"},
         {"use_up",
          "continue refining the equivalence class until "
          "the size limit is hit"}});

    add_shrink_strategy_bisimulation_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink
