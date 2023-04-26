#include "probfd/heuristics/pdbs/cegar/bfs_flaw_finder.h"

#include "probfd/heuristics/pdbs/cegar/cegar.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "utils/collections.h"
#include "utils/countdown_timer.h"

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

BFSFlawFinder::BFSFlawFinder(options::Options& opts)
    : BFSFlawFinder(opts.get<int>("max_search_states"))
{
}

BFSFlawFinder::BFSFlawFinder(int max_search_states)
    : closed(false)
    , max_search_states(max_search_states)
{
}

bool BFSFlawFinder::apply_policy(
    CEGAR& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    std::vector<Flaw>& flaw_list,
    utils::CountdownTimer& timer)
{
    assert(open.empty() && closed.empty());

    // Exception safety due to TimeoutException
    scope_exit guard([&]() {
        open.clear();
        closed.clear();
    });

    StateRegistry registry(task_proxy);
    const State& init = registry.get_initial_state();

    open.push_back(init);
    closed[init.get_id()] = true;

    do {
        timer.throw_if_expired();

        State current = std::move(open.front());
        open.pop_front();

        if (!expand(
                base,
                task_proxy,
                solution_index,
                std::move(current),
                flaw_list,
                registry)) {
            return false;
        }
    } while (!open.empty());

    return true;
}

std::string BFSFlawFinder::get_name()
{
    return "BFS Flaw Finder";
}

bool BFSFlawFinder::expand(
    CEGAR& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    State state,
    std::vector<Flaw>& flaw_list,
    StateRegistry& registry)
{
    PDBInfo& solution = *base.pdb_infos[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const ProbabilisticPatternDatabase& pdb = solution.get_pdb();

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
                    utils::contains(base.goals, goal_var)) {
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
        ProbabilisticOperatorProxy op = operators[abs_op->operator_id];

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

            if (static_cast<int>(registry.size()) > max_search_states) {
                return false;
            }

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

static std::shared_ptr<FlawFindingStrategy>
_parse(options::OptionParser& parser)
{
    parser.add_option<int>(
        "max_search_states",
        "Maximal number of generated states after which the flaw search is "
        "aborted.",
        "20M",
        options::Bounds("0", "infinity"));

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return std::make_shared<BFSFlawFinder>(opts);
}

static Plugin<FlawFindingStrategy> _plugin("bfs_flaw_finder", _parse);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd