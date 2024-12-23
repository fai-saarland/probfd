#include "downward_plugins/pdbs/cegar_options.h"

#include "downward_plugins/plugins/plugin.h"

using namespace std;

namespace downward_plugins::pdbs {

void add_cegar_implementation_notes_to_feature(plugins::Feature& feature)
{
    feature.document_note(
        "Short description of the CEGAR algorithm",
        "The CEGAR algorithm computes a pattern collection for a given "
        "planning "
        "task and a given (sub)set of its goals in a randomized order as "
        "follows. Starting from the pattern collection consisting of a "
        "singleton "
        "pattern for each goal variable, it repeatedly attempts to execute an "
        "optimal plan of each pattern in the concrete task, collects reasons "
        "why "
        "this is not possible (so-called flaws) and refines the pattern in "
        "question by adding a variable to it.\n"
        "Further parameters allow blacklisting a (sub)set of the non-goal "
        "variables which are then never added to the collection, limiting PDB "
        "and collection size, setting a time limit and switching between "
        "computing regular or wildcard plans, where the latter are sequences "
        "of "
        "parallel operators inducing the same abstract transition.",
        true);
    feature.document_note(
        "Implementation notes about the CEGAR algorithm",
        "The following describes differences of the implementation to "
        "the original implementation used and described in the paper.\n\n"
        "Conceptually, there is one larger difference which concerns the "
        "computation of (regular or wildcard) plans for PDBs. The original "
        "implementation used an enforced hill-climbing (EHC) search with the "
        "PDB as the perfect heuristic, which ensured finding strongly optimal "
        "plans, i.e., optimal plans with a minimum number of zero-cost "
        "operators, in domains with zero-cost operators. The original "
        "implementation also slightly modified EHC to search for a best-"
        "improving successor, chosen uniformly at random among all best-"
        "improving successors.\n\n"
        "In contrast, the current implementation computes a plan alongside the "
        "computation of the PDB itself. A modification to Dijkstra's algorithm "
        "for computing the PDB values stores, for each state, the operator "
        "leading to that state (in a regression search). This generating "
        "operator is updated only if the algorithm found a cheaper path to "
        "the state. After Dijkstra finishes, the plan computation starts at "
        "the "
        "initial state and iteratively follows the generating operator, "
        "computes "
        "all operators of the same cost inducing the same transition, until "
        "reaching a goal. This constitutes a wildcard plan. It is turned into "
        "a "
        "regular one by randomly picking a single operator for each "
        "transition. "
        "\n\n"
        "Note that this kind of plan extraction does not consider all "
        "successors "
        "of a state uniformly at random but rather uses the previously "
        "deterministically "
        "chosen generating operator to settle on one successor state, which is "
        "biased by the number of operators leading to the same successor from "
        "the given state. Further note that in the presence of zero-cost "
        "operators, this procedure does not guarantee that the computed plan "
        "is "
        "strongly optimal because it does not minimize the number of used "
        "zero-cost operators leading to the state when choosing a generating "
        "operator. Experiments have shown (issue1007) that this speeds up the "
        "computation significantly while not having a strongly negative effect "
        "on heuristic quality due to potentially computing worse plans.\n\n"
        "Two further changes fix bugs of the original implementation to match "
        "the description in the paper. The first bug fix is to raise a flaw "
        "for all goal variables of the task if the plan for a PDB can be "
        "executed on the concrete task but does not lead to a goal state. "
        "Previously, such flaws would not have been raised because all goal "
        "variables are part of the collection from the start on and therefore "
        "not considered. This means that the original implementation "
        "accidentally disallowed merging patterns due to goal violation "
        "flaws. The second bug fix is to actually randomize the order of "
        "parallel operators in wildcard plan steps.",
        true);
}

void add_cegar_wildcard_option_to_feature(plugins::Feature& feature)
{
    feature.add_option<bool>(
        "use_wildcard_plans",
        "if true, compute wildcard plans which are sequences of sets of "
        "operators that induce the same transition; otherwise compute regular "
        "plans which are sequences of single operators",
        "true");
}

tuple<bool>
get_cegar_wildcard_arguments_from_options(const plugins::Options& opts)
{
    return make_tuple(opts.get<bool>("use_wildcard_plans"));
}

} // namespace downward_plugins::pdbs
