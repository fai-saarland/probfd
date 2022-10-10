#include "bfs_flaw_finder.h"

#include "abstract_solution_data.h"
#include "pattern_collection_generator_cegar.h"

#include "../../abstract_state.h"
#include "../../expcost_projection.h"
#include "../../maxprob_projection.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

#include "../../../../../utils/collections.h"

using namespace std;
using namespace utils;

namespace {
static const std::string token = "CEGAR_PDBs: ";
}

namespace probabilistic {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
BFSFlawFinder<PDBType>::BFSFlawFinder(options::Options& opts)
    : BFSFlawFinder<PDBType>(opts.get<int>("violation_threshold"))
{
}

template <typename PDBType>
BFSFlawFinder<PDBType>::BFSFlawFinder(unsigned int violation_threshold)
    : violation_threshold(violation_threshold)
{
}

template <typename PDBType>
std::pair<FlawList, bool> BFSFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    const ExplicitGState& init)
{
    assert(open.empty() && closed.empty());

    FlawList flaw_list;
    bool executable = true;
    unsigned int violations = 0;

    open.push_back(init);
    closed.insert(init.values);

    while (!open.empty()) {
        ExplicitGState current = open.front();
        open.pop_front();

        bool successful = expand(base, solution_index, current, flaw_list);
        executable = executable && successful;

        if (!successful && ++violations >= violation_threshold) {
            break;
        }
    }

    open.clear();
    closed.clear();
    return {flaw_list, executable};
}

template <typename PDBType>
bool BFSFlawFinder<PDBType>::expand(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    ExplicitGState state,
    FlawList& flaw_list)
{
    AbstractSolutionData<PDBType>& solution = *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    const AbstractState abs = pdb.get_abstract_state(state.values);

    // We reached a dead-end, the operator is irrelevant.
    if (pdb.is_dead_end(abs)) {
        return true;
    }

    const auto abs_op_it = policy.find(abs);

    // We reached a terminal state, check if it is a goal
    if (abs_op_it == policy.end()) {
        assert(pdb.is_goal(abs));

        if (pdb.is_goal(abs) && !state.is_goal()) {
            if (!base.ignore_goal_violations) {
                // Collect all non-satisfied goal variables that are still
                // available.
                for (const auto& [goal_var, goal_value] : g_goal) {
                    if (state[goal_var] != goal_value &&
                        !utils::contains(base.global_blacklist, goal_var) &&
                        utils::contains(base.remaining_goals, goal_var)) {
                        flaw_list.emplace_back(true, solution_index, goal_var);
                    }
                }

                return false;
            }
        }

        return true;
    }

    const AbstractPolicy::OperatorList& abs_operators = abs_op_it->second;
    FlawList local_flaws;

    for (const AbstractOperator* abs_op : abs_operators) {
        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperator& op = g_operators[original_id];

        // Check whether all preconditions are fulfilled
        bool preconditions_ok = true;

        for (const auto& [pre_var, pre_val] : op.get_preconditions()) {
            // We ignore blacklisted variables
            const bool is_blacklist_var =
                utils::contains(base.global_blacklist, pre_var);

            if (is_blacklist_var || solution.is_blacklisted(pre_var)) {
                assert(
                    !solution.is_blacklisted(pre_var) ||
                    base.local_blacklisting);
                continue;
            }

            if (state[pre_var] != pre_val) {
                preconditions_ok = false;
                local_flaws.emplace_back(false, solution_index, pre_var);
            }
        }

        // Flaws occured.
        if (!preconditions_ok) {
            continue; // Try next operator
        }

        // Generate the successors and add them to the open list
        for (const auto& outcome : op) {
            ExplicitGState successor = state.get_successor(*outcome.op);

            if (!utils::contains(closed, successor)) {
                closed.insert(successor);
                open.push_back(successor);
            }
        }

        return true;
    }

    // Insert all flaws of all operators
    flaw_list.insert(flaw_list.end(), local_flaws.begin(), local_flaws.end());

    return false;
}

template <typename PDBType>
static std::shared_ptr<FlawFindingStrategy<PDBType>>
_parse(options::OptionParser& parser)
{
    parser.add_option<int>(
        "violation_threshold",
        "Maximal number of states for which a flaw is tolerated before aborting"
        "the search.",
        "1",
        options::Bounds("0", "infinity"));

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<BFSFlawFinder<PDBType>>(opts);
}

static Plugin<FlawFindingStrategy<MaxProbProjection>>
    _plugin_maxprob("bfs_flaw_finder_mp", _parse<MaxProbProjection>);
static Plugin<FlawFindingStrategy<ExpCostProjection>>
    _plugin_expcost("bfs_flaw_finder_ec", _parse<ExpCostProjection>);

template class BFSFlawFinder<MaxProbProjection>;
template class BFSFlawFinder<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probabilistic