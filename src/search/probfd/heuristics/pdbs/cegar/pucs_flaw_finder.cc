#include "probfd/heuristics/pdbs/cegar/pucs_flaw_finder.h"

#include "probfd/heuristics/pdbs/cegar/cegar.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "utils/collections.h"
#include "utils/countdown_timer.h"

#include "state_registry.h"

#include "option_parser.h"
#include "plugin.h"

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

PUCSFlawFinder::PUCSFlawFinder(options::Options& opts)
    : PUCSFlawFinder(opts.get<int>("max_search_states"))
{
}

PUCSFlawFinder::PUCSFlawFinder(int max_search_states)
    : max_search_states(max_search_states)
{
}

bool PUCSFlawFinder::apply_policy(
    const CEGAR& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    std::vector<Flaw>& flaw_list,
    utils::CountdownTimer& timer)
{
    assert(pq.empty() && probabilities.empty());

    // Exception safety due to TimeoutException
    scope_exit guard([&] {
        pq.clear();
        probabilities.clear();
    });

    StateRegistry registry(task_proxy);

    {
        const State& init = registry.get_initial_state();
        pq.push(1.0, init);
        probabilities[StateID(init.get_id())].path_probability = 1.0;
    }

    const PDBInfo& solution = *base.pdb_infos[solution_index];
    const ProjectionPolicy& policy = solution.get_policy();
    const ProbabilisticPatternDatabase& pdb = solution.get_pdb();
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy goals = task_proxy.get_goals();

    do {
        timer.throw_if_expired();

        auto [path_probability, current] = pq.pop();
        auto& info = probabilities[StateID(current.get_id())];
        assert(!info.expanded);

        // TODO remove this once we have a real priority queue...
        if (path_probability < info.path_probability) {
            continue;
        }

        info.expanded = true;

        assert(path_probability != 0_vt);

        // Check flaws, generate successors
        const StateRank abs = pdb.get_abstract_state(current);

        // We reached a dead-end, the operator is irrelevant.
        if (pdb.is_dead_end(abs)) {
            continue;
        }

        const auto& abs_operators = policy[abs];

        // We reached a terminal state, check if it is a goal
        if (abs_operators.empty()) {
            assert(solution.is_goal(abs));

            return base.collect_flaws(
                goals,
                current,
                solution_index,
                false,
                flaw_list);
        }

        std::vector<Flaw> local_flaws;

        for (const ProjectionOperator* abs_op : abs_operators) {
            const auto op = operators[abs_op->operator_id];

            if (base.collect_flaws(
                    op.get_preconditions(),
                    current,
                    solution_index,
                    true,
                    local_flaws)) {
                continue; // Try next operator
            }

            // Generate the successors and add them to the queue
            for (const auto outcome : op.get_outcomes()) {
                State successor =
                    registry.get_successor_state(current, outcome);

                if (static_cast<int>(registry.size()) > max_search_states) {
                    return false;
                }

                auto& succ_entry = probabilities[StateID(successor.get_id())];
                const auto succ_prob =
                    path_probability * outcome.get_probability();

                if (!succ_entry.expanded &&
                    succ_entry.path_probability < succ_prob) {
                    succ_entry.path_probability = succ_prob;
                    pq.push(succ_prob, std::move(successor));
                }
            }

            goto continue_exploration;
        }

        // Insert all flaws of all operators
        flaw_list.insert(
            flaw_list.end(),
            local_flaws.begin(),
            local_flaws.end());

        return false;

    continue_exploration:;
    } while (!pq.empty());

    return true;
}

std::string PUCSFlawFinder::get_name()
{
    return "PUCS Flaw Finder";
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

    return make_shared<PUCSFlawFinder>(opts);
}

static Plugin<FlawFindingStrategy> _plugin("pucs_flaw_finder", _parse);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd