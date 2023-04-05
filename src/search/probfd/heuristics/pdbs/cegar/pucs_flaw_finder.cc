#include "probfd/heuristics/pdbs/cegar/pucs_flaw_finder.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/pattern_collection_generator_cegar.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include "probfd/task_utils/task_properties.h"

#include "utils/collections.h"

#include "state_registry.h"

#include "option_parser.h"
#include "plugin.h"

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
PUCSFlawFinder<PDBType>::PUCSFlawFinder(options::Options& opts)
    : PUCSFlawFinder(opts.get<int>("violation_threshold"))
{
}

template <typename PDBType>
PUCSFlawFinder<PDBType>::PUCSFlawFinder(unsigned int violation_threshold)
    : probabilities(0_vt)
    , violation_threshold(violation_threshold)
{
}

template <typename PDBType>
bool PUCSFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    std::vector<Flaw>& flaw_list)
{
    assert(pq.empty() && probabilities.empty());

    StateRegistry registry(task_proxy);
    const State& init = registry.get_initial_state();

    // Push initial state for expansion
    pq.push(1.0, init);
    probabilities[StateID(init.get_id())] = 1.0;

    unsigned int violations = 0;
    bool executable = true;

    while (!pq.empty()) {
        auto [priority, current] = pq.pop();

        // TODO remove this once we have a real priority queue...
        if (priority < probabilities[StateID(current.get_id())]) {
            continue;
        }

        bool successful = expand(
            base,
            task_proxy,
            solution_index,
            std::move(current),
            priority,
            flaw_list,
            registry);

        executable = executable && successful;

        // Check if a flaw occured.
        if (!successful && ++violations >= violation_threshold) {
            break;
        }
    }

    pq.clear();
    probabilities.clear();
    return executable;
}

template <typename PDBType>
std::string PUCSFlawFinder<PDBType>::get_name()
{
    return "PUCS Flaw Finder";
}

template <typename PDBType>
bool PUCSFlawFinder<PDBType>::expand(
    PatternCollectionGeneratorCegar<PDBType>& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    State state,
    value_t priority,
    std::vector<Flaw>& flaw_list,
    StateRegistry& registry)
{
    AbstractSolutionData<PDBType>& solution = *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    // Check flaws, generate successors
    const StateRank abs = pdb.get_abstract_state(state);

    // We reached a dead-end, the operator is irrelevant.
    if (pdb.is_dead_end(abs)) {
        return true;
    }

    const auto& abs_operators = policy[abs];

    // We reached a terminal state, check if it is a goal
    if (abs_operators.empty()) {
        assert(solution.is_goal(abs));

        if (!::task_properties::is_goal_state(task_proxy, state)) {
            if (!base.ignore_goal_violations) {
                // Collect all non-satisfied goal variables that are still
                // available.
                for (FactProxy fact : task_proxy.get_goals()) {
                    const auto& [goal_var, goal_val] = fact.get_pair();

                    if (state[goal_var].get_value() != goal_val &&
                        !base.global_blacklist.contains(goal_var) &&
                        utils::contains(base.remaining_goals, goal_var)) {
                        flaw_list.emplace_back(true, solution_index, goal_var);
                    }
                }
            }

            return false;
        }

        return true;
    }

    std::vector<Flaw> local_flaws;

    const auto operators = task_proxy.get_operators();

    for (const AbstractOperator* abs_op : abs_operators) {
        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperatorProxy& op = operators[original_id];

        // Check whether precondition flaws occur
        bool preconditions_ok = true;

        for (const FactProxy precondition : op.get_preconditions()) {
            const auto& [pre_var, pre_val] = precondition.get_pair();
            // We ignore blacklisted variables
            const bool is_blacklist_var =
                base.global_blacklist.contains(pre_var);

            if (is_blacklist_var || solution.is_blacklisted(pre_var)) {
                assert(
                    !solution.is_blacklisted(pre_var) ||
                    base.local_blacklisting);
                continue;
            }

            if (state[pre_var].get_value() != pre_val) {
                preconditions_ok = false;
                local_flaws.emplace_back(false, solution_index, pre_var);
            }
        }

        // Flaws occured.
        if (!preconditions_ok) {
            continue; // Try next operator
        }

        // Generate the successors and add them to the queue
        for (const ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            const auto succ_prob = priority * outcome.get_probability();
            State successor = registry.get_successor_state(state, outcome);
            value_t& succ_entry = probabilities[StateID(successor.get_id())];

            if (succ_entry != 0_vt) {
                if (succ_prob <= succ_entry) {
                    continue;
                }

                succ_entry = succ_prob;
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

static Plugin<FlawFindingStrategy<MaxProbPatternDatabase>>
    _plugin_maxprob("pucs_flaw_finder_mp", _parse<MaxProbPatternDatabase>);
static Plugin<FlawFindingStrategy<SSPPatternDatabase>>
    _plugin_expcost("pucs_flaw_finder_ec", _parse<SSPPatternDatabase>);

template class PUCSFlawFinder<MaxProbPatternDatabase>;
template class PUCSFlawFinder<SSPPatternDatabase>;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd