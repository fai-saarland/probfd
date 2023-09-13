#include "probfd/heuristics/pdbs/cegar/mlp_exploration_strategy.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_info.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "probfd/multi_policy.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"

#include "downward/state_registry.h"

#include "downward/plugins/plugin.h"

#include <ranges>

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

MLPExplorationStrategy::MLPExplorationStrategy(const plugins::Options& opts)
    : MLPExplorationStrategy(opts.get<int>("max_search_states"))
{
}

MLPExplorationStrategy::MLPExplorationStrategy(int max_search_states)
    : max_search_states(max_search_states)
{
}

bool MLPExplorationStrategy::apply_policy(
    ProbabilisticTaskProxy task_proxy,
    const ProjectionInfo& projection,
    const ProjectionMultiPolicy& policy,
    FlawFilter& flaw_filter,
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

    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy goals = task_proxy.get_goals();

    bool flaws_found = false;

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
        const AbstractStateIndex abs = projection.get_abstract_state(current);

        const std::vector abs_decisions = policy.get_decisions(abs);

        // We reached a terminal state, check if it is a goal or dead-end
        if (abs_decisions.empty()) {
            if (projection.is_goal(abs)) {
                const size_t flaws_before = flaw_list.size();
                flaws_found =
                    collect_flaws(goals, current, flaw_filter, flaw_list) ||
                    flaws_found;

                if (flaws_before != flaw_list.size()) {
                    return false;
                }

                continue;
            }
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

            if (flaws_before != local_flaws.size()) {
                // Try next operator
                continue;
            }

            flaws_found = flaws_found || local_flaws_found;

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

    return !flaws_found;
}

std::string MLPExplorationStrategy::get_name()
{
    return "Most-Likely Path";
}

class MLPExplorationStrategyFeature
    : public plugins::
          TypedFeature<PolicyExplorationStrategy, MLPExplorationStrategy> {
public:
    MLPExplorationStrategyFeature()
        : TypedFeature("mlp")
    {
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            plugins::Bounds("0", "infinity"));
    }
};

static plugins::FeaturePlugin<MLPExplorationStrategyFeature> _plugin;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd