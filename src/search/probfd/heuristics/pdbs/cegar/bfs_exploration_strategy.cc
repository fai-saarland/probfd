#include "probfd/heuristics/pdbs/cegar/bfs_exploration_strategy.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"

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

BFSExplorationStrategy::BFSExplorationStrategy(const plugins::Options& opts)
    : BFSExplorationStrategy(opts.get<int>("max_search_states"))
{
}

BFSExplorationStrategy::BFSExplorationStrategy(int max_search_states)
    : closed(false)
    , max_search_states(max_search_states)
{
}

bool BFSExplorationStrategy::apply_policy(
    ProbabilisticTaskProxy task_proxy,
    const ProjectionStateSpace& mdp,
    const ProbabilityAwarePatternDatabase& pdb,
    const ProjectionMultiPolicy& policy,
    FlawFilter& flaw_filter,
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

    bool flaws_found = false;

    do {
        timer.throw_if_expired();

        const State& current = open.front();
        const AbstractStateIndex abs = pdb.get_abstract_state(current);

        {
            const std::vector abs_decisions = policy.get_decisions(abs);

            // We reached a terminal state, check if it is a goal or dead-end
            if (abs_decisions.empty()) {
                if (mdp.is_goal(abs)) {
                    const size_t flaws_before = flaw_list.size();
                    flaws_found =
                        collect_flaws(goals, current, flaw_filter, flaw_list) ||
                        flaws_found;

                    if (flaws_before != flaw_list.size()) {
                        return false;
                    }
                }

                goto continue_exploration;
            }

            std::vector<Flaw> local_flaws;

            for (const auto& decision : abs_decisions) {
                const auto* abs_op = decision.action;
                const auto op = operators[abs_op->operator_id];

                const size_t flaws_before = local_flaws.size();
                const bool local_flaws_found = collect_flaws(
                    op.get_preconditions(),
                    current,
                    flaw_filter,
                    local_flaws);

                // Flaws occured.
                if (flaws_before != local_flaws.size()) {
                    continue; // Try next operator
                }

                flaws_found = flaws_found || local_flaws_found;

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

    return !flaws_found;
}

std::string BFSExplorationStrategy::get_name()
{
    return "Breadth-First Search";
}

class BFSExplorationStrategyFeature
    : public plugins::
          TypedFeature<PolicyExplorationStrategy, BFSExplorationStrategy> {
public:
    BFSExplorationStrategyFeature()
        : TypedFeature("bfs")
    {
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            plugins::Bounds("0", "infinity"));
    }
};

static plugins::FeaturePlugin<BFSExplorationStrategyFeature> _plugin;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd