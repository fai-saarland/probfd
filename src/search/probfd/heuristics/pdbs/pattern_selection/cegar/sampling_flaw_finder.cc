#include "probfd/heuristics/pdbs/pattern_selection/cegar/sampling_flaw_finder.h"

#include "probfd/heuristics/pdbs/pattern_selection/cegar/abstract_solution_data.h"
#include "probfd/heuristics/pdbs/pattern_selection/cegar/pattern_collection_generator_cegar.h"

#include "probfd/heuristics/pdbs/abstract_state.h"
#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include "probfd/distribution.h"

#include "utils/collections.h"
#include "utils/rng.h"

#include "option_parser.h"
#include "plugin.h"

#include <stack>

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
SamplingFlawFinder<PDBType>::SamplingFlawFinder(options::Options& opts)
    : SamplingFlawFinder<PDBType>(opts.get<int>("violation_threshold"))
{
}

template <typename PDBType>
SamplingFlawFinder<PDBType>::SamplingFlawFinder(
    unsigned int violation_threshold)
    : violation_threshold(violation_threshold)
{
}

template <typename PDBType>
std::pair<FlawList, bool> SamplingFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    const ExplicitGState& init)
{
    assert(stk.empty() && einfos.empty());

    FlawList flaw_list;
    bool executable = true;
    unsigned int violation = 0;

    int status = push_state(base, solution_index, init, flaw_list);
    if ((status & STATE_PUSHED) == 0) {
        assert(stk.empty() && einfos.empty());
        return {flaw_list, (status & FLAW_OCCURED) == 0};
    }

    assert(!stk.empty());

    do {
        const ExplicitGState& current = stk.top();
        ExplorationInfo& einfo = einfos[current];

        while (!einfo.successors.empty()) {
            // Sample next successor
            auto it = einfo.successors.sample(*base.rng);
            auto& succ = it->first;

            // Ignore states already seen
            if (einfos.find(succ) == einfos.end()) {
                unsigned int status =
                    push_state(base, solution_index, succ, flaw_list);

                // Recurse if the state was pushed
                if (status & STATE_PUSHED) {
                    einfo.successors.erase(it);
                    goto continue_exploration;
                }

                // Otherwise, check if the state was not pushed due to a flaw
                if (status & FLAW_OCCURED) {
                    executable = false;

                    if (++violation >= violation_threshold) {
                        goto break_exploration;
                    }
                }
            }

            // Don't sample it again
            einfo.successors.erase(it);
        }

        stk.pop();

    continue_exploration:;
    } while (!stk.empty());

break_exploration:;

    std::stack<ExplicitGState>().swap(stk); // Clear stack
    einfos.clear();
    return {flaw_list, executable};
}

template <typename PDBType>
unsigned int SamplingFlawFinder<PDBType>::push_state(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    const ExplicitGState& state,
    FlawList& flaw_list)
{
    AbstractSolutionData<PDBType>& solution = *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    // Check flaws, generate successors
    const AbstractState abs = pdb.get_abstract_state(state.values);

    // We reached a dead-end, the operator is irrelevant.
    if (pdb.is_dead_end(abs)) {
        return 0;
    }

    const auto abs_op_it = policy.find(abs);

    // We reached a terminal state, check if it is a goal
    if (abs_op_it == policy.end()) {
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

            return FLAW_OCCURED;
        }

        return 0;
    }

    const AbstractPolicy::OperatorList& abs_operators = abs_op_it->second;
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

        // Generate the successors
        assert(einfos.find(state) == einfos.end());
        ExplorationInfo& einfo = einfos[state];
        for (const auto& [det_op, prob] : op) {
            einfo.successors.add_unique(state.get_successor(*det_op), prob);
        }

        stk.push(state);
        return STATE_PUSHED;
    }

    // Insert all flaws of all operators
    flaw_list.insert(flaw_list.end(), local_flaws.begin(), local_flaws.end());

    return FLAW_OCCURED;
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

    return make_shared<SamplingFlawFinder<PDBType>>(opts);
}

static Plugin<FlawFindingStrategy<MaxProbProjection>>
    _plugin_maxprob("sampling_flaw_finder_mp", _parse<MaxProbProjection>);
static Plugin<FlawFindingStrategy<ExpCostProjection>>
    _plugin_expcost("sampling_flaw_finder_ec", _parse<ExpCostProjection>);

template class SamplingFlawFinder<MaxProbProjection>;
template class SamplingFlawFinder<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd