#include "probfd/heuristics/pdbs/cegar/bfs_flaw_finder.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/multi_policy.h"

#include "probfd/utils/guards.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"

#include "downward/state_id.h"
#include "downward/state_registry.h"

#include "downward/plugins/plugin.h"

#include <ranges>

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

BFSFlawFinder::BFSFlawFinder(const plugins::Options& opts)
    : BFSFlawFinder(opts.get<int>("max_search_states"))
{
}

BFSFlawFinder::BFSFlawFinder(int max_search_states)
    : closed(false)
    , max_search_states(max_search_states)
{
}

bool BFSFlawFinder::apply_policy(
    const ProbabilisticTaskProxy& task_proxy,
    const ProjectionStateSpace& mdp,
    const StateRankingFunction& abstraction_mapping,
    const ProjectionMultiPolicy& policy,
    std::function<bool(int)> ignore_flaw,
    std::vector<Flaw>& flaw_list,
    utils::CountdownTimer& timer)
{
    assert(open.empty() && closed.empty());

    // Exception safety due to TimeoutException
    scope_exit guard([&] {
        open.clear();
        closed.clear();
    });

    StateRegistry registry(task_proxy);

    {
        const State& init = registry.get_initial_state();
        open.push_back(init);
        closed[init.get_id()] = true;
    }

    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy goals = task_proxy.get_goals();

    const int pdb_size = abstraction_mapping.num_states();

    do {
        timer.throw_if_expired();

        const State& current = open.front();
        const StateRank abs = abstraction_mapping.get_abstract_rank(current);

        {
            const std::vector abs_decisions = policy.get_decisions(abs);

            // We reached a terminal state, check if it is a goal or dead-end
            if (abs_decisions.empty()) {
                if (mdp.is_goal(abs) &&
                    collect_flaws(goals, current, ignore_flaw, flaw_list))
                    return false;

                goto continue_exploration;
            }

            std::vector<Flaw> local_flaws;

            for (const auto& decision : abs_decisions) {
                const auto* abs_op = decision.action;
                const auto op = operators[abs_op->operator_id];

                // Flaws occured.
                if (collect_flaws(
                        op.get_preconditions(),
                        current,
                        ignore_flaw,
                        local_flaws)) {
                    continue; // Try next operator
                }

                // Generate the successors and add them to the open list
                for (const auto outcome : op.get_outcomes()) {
                    State successor =
                        registry.get_successor_state(current, outcome);

                    if (static_cast<int>(registry.size()) > max_search_states) {
                        return false;
                    }

                    auto seen = closed[StateID(successor.get_id())];

                    if (!seen) {
                        seen = true;
                        open.push_back(std::move(successor));
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
        }

    continue_exploration:;
        open.pop_front();

    } while (!open.empty());

    return true;
}

std::string BFSFlawFinder::get_name()
{
    return "BFS Flaw Finder";
}

class BFSFlawFinderFeature
    : public plugins::TypedFeature<FlawFindingStrategy, BFSFlawFinder> {
public:
    BFSFlawFinderFeature()
        : TypedFeature("bfs_flaw_finder")
    {
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            plugins::Bounds("0", "infinity"));
    }
};

static plugins::FeaturePlugin<BFSFlawFinderFeature> _plugin;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd