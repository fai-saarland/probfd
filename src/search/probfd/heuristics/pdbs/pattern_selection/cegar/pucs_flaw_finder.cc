#include "probfd/heuristics/pdbs/pattern_selection/cegar/pucs_flaw_finder.h"

#include "probfd/heuristics/pdbs/pattern_selection/cegar/abstract_solution_data.h"
#include "probfd/heuristics/pdbs/pattern_selection/cegar/pattern_collection_generator_cegar.h"

#include "probfd/heuristics/pdbs/abstract_state.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"
#include "probfd/heuristics/pdbs/expcost_projection.h"

#include "utils/collections.h"

#include "option_parser.h"
#include "plugin.h"

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
PUCSFlawFinder<PDBType>::PUCSFlawFinder(options::Options& opts)
    : PUCSFlawFinder<PDBType>(opts.get<int>("violation_threshold"))
{
}

template <typename PDBType>
PUCSFlawFinder<PDBType>::PUCSFlawFinder(unsigned int violation_threshold)
    : violation_threshold(violation_threshold)
{
}

template <typename PDBType>
std::pair<FlawList, bool> PUCSFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    const ExplicitGState& init)
{
    assert(pq.empty() && probabilities.empty());

    // Push initial state for expansion
    pq.push(1.0, init);
    probabilities[init] = 1.0;

    FlawList flaw_list;
    unsigned int violations = 0;
    bool executable = true;

    while (!pq.empty()) {
        const auto& [priority, current] = pq.pop();

        // TODO remove this once we have a real priority queue...
        if (priority < probabilities[current]) {
            continue;
        }

        bool successful =
            expand(base, solution_index, current, priority, flaw_list);

        executable = executable && successful;

        // Check if a flaw occured.
        if (!successful && ++violations >= violation_threshold) {
            break;
        }
    }

    pq.clear();
    probabilities.clear();
    return {flaw_list, executable};
}

template <typename PDBType>
bool PUCSFlawFinder<PDBType>::expand(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    ExplicitGState state,
    value_type::value_t priority,
    FlawList& flaw_list)
{
    AbstractSolutionData<PDBType>& solution = *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    // Check flaws, generate successors
    const AbstractState abs = pdb.get_abstract_state(state.values);

    // We reached a dead-end, the operator is irrelevant.
    if (pdb.is_dead_end(abs)) {
        return true;
    }

    const auto& abs_operators = policy[abs];

    // We reached a terminal state, check if it is a goal
    if (abs_operators.empty()) {
        assert(pdb.is_goal(abs));

        if (pdb.is_goal(abs) && !state.is_goal()) {
            if (!base.ignore_goal_violations) {
                // Collect all non-satisfied goal variables that are
                // still available.
                for (const auto& [goal_var, goal_value] : g_goal) {
                    if (state[goal_var] != goal_value &&
                        !utils::contains(base.global_blacklist, goal_var) &&
                        utils::contains(base.remaining_goals, goal_var)) {
                        flaw_list.emplace_back(true, solution_index, goal_var);
                    }
                }
            }

            return false;
        }

        return true;
    }

    FlawList local_flaws;

    for (const AbstractOperator* abs_op : abs_operators) {
        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperator& op = g_operators[original_id];

        // Check whether precondition flaws occur
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

        // Generate the successors and add them to the queue
        for (const auto& [det_op, prob] : op) {
            const value_type::value_t succ_prob = priority * prob;
            auto successor = state.get_successor(*det_op);

            auto [it, inserted] = probabilities.emplace(successor, succ_prob);

            if (!inserted) {
                if (succ_prob <= it->second) {
                    continue;
                }

                it->second = succ_prob;
            }

            pq.push(succ_prob, std::move(successor));
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

    return make_shared<PUCSFlawFinder<PDBType>>(opts);
}

static Plugin<FlawFindingStrategy<MaxProbProjection>>
    _plugin_maxprob("pucs_flaw_finder_mp", _parse<MaxProbProjection>);
static Plugin<FlawFindingStrategy<ExpCostProjection>>
    _plugin_expcost("pucs_flaw_finder_ec", _parse<ExpCostProjection>);

template class PUCSFlawFinder<MaxProbProjection>;
template class PUCSFlawFinder<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd