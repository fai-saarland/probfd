#include "probfd/heuristics/pdbs/cegar/bfs_flaw_finder.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/pattern_collection_generator_cegar.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"

#include "utils/collections.h"

#include "probfd/task_utils/task_properties.h"

#include "state_id.h"
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
BFSFlawFinder<PDBType>::BFSFlawFinder(options::Options& opts)
    : BFSFlawFinder(opts.get<int>("violation_threshold"))
{
}

template <typename PDBType>
BFSFlawFinder<PDBType>::BFSFlawFinder(unsigned violation_threshold)
    : closed(false)
    , violation_threshold(violation_threshold)
{
}

template <typename PDBType>
bool BFSFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    std::vector<Flaw>& flaw_list)
{
    assert(open.empty() && closed.empty());

    StateRegistry registry(task_proxy);
    const State& init = registry.get_initial_state();

    bool executable = true;
    unsigned int violations = 0;

    open.push_back(init);
    closed[init.get_id()] = true;

    do {
        State current = std::move(open.front());
        open.pop_front();

        bool successful = expand(
            base,
            task_proxy,
            solution_index,
            std::move(current),
            flaw_list,
            registry);
        executable = executable && successful;

        if (!successful && ++violations >= violation_threshold) {
            open.clear();
            break;
        }
    } while (!open.empty());

    closed.clear();
    return executable;
}

template <typename PDBType>
std::string BFSFlawFinder<PDBType>::get_name()
{
    return "BFS Flaw Finder";
}

template <typename PDBType>
bool BFSFlawFinder<PDBType>::expand(
    PatternCollectionGeneratorCegar<PDBType>& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    State state,
    std::vector<Flaw>& flaw_list,
    StateRegistry& registry)
{
    PDBInfo<PDBType>& solution = *base.pdb_infos[solution_index];
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
            // Collect all non-satisfied goal variables that are still
            // available.
            for (FactProxy fact : task_proxy.get_goals()) {
                const int goal_var = fact.get_variable().get_id();
                const int goal_val = fact.get_value();

                if (state[goal_var].get_value() != goal_val &&
                    !base.blacklisted_variables.contains(goal_var) &&
                    utils::contains(base.remaining_goals, goal_var)) {
                    flaw_list.emplace_back(solution_index, goal_var);
                }
            }

            return false;
        }

        return true;
    }

    std::vector<Flaw> local_flaws;

    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();

    for (const AbstractOperator* abs_op : abs_operators) {
        int original_id = abs_op->original_operator_id;
        ProbabilisticOperatorProxy op = operators[original_id];

        // Check whether all preconditions are fulfilled
        bool preconditions_ok = true;

        for (const FactProxy fact : op.get_preconditions()) {
            const int pre_var = fact.get_variable().get_id();
            const int pre_val = fact.get_value();

            // We ignore blacklisted variables
            if (base.blacklisted_variables.contains(pre_var)) {
                continue;
            }

            if (state[pre_var].get_value() != pre_val) {
                preconditions_ok = false;
                local_flaws.emplace_back(solution_index, pre_var);
            }
        }

        // Flaws occured.
        if (!preconditions_ok) {
            continue; // Try next operator
        }

        // Generate the successors and add them to the open list
        for (const ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            State successor = registry.get_successor_state(state, outcome);
            auto seen = closed[StateID(successor.get_id())];

            if (!seen) {
                seen = true;
                open.push_back(std::move(successor));
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

    return std::make_shared<BFSFlawFinder<PDBType>>(opts);
}

static Plugin<FlawFindingStrategy<MaxProbPatternDatabase>>
    _plugin_maxprob("bfs_flaw_finder_mp", _parse<MaxProbPatternDatabase>);
static Plugin<FlawFindingStrategy<SSPPatternDatabase>>
    _plugin_expcost("bfs_flaw_finder_ec", _parse<SSPPatternDatabase>);

template class BFSFlawFinder<MaxProbPatternDatabase>;
template class BFSFlawFinder<SSPPatternDatabase>;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd