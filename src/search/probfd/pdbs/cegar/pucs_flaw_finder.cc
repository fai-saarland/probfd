#include "probfd/pdbs/cegar/pucs_flaw_finder.h"

#include "probfd/pdbs/cegar/flaw.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/types.h"

#include "probfd/utils/guards.h"

#include "probfd/multi_policy.h"
#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"

#include "downward/state_registry.h"

#include "downward/plugins/plugin.h"

#include <cassert>

using namespace std;
using namespace utils;

namespace probfd::pdbs::cegar {

PUCSFlawFinder::PUCSFlawFinder(const plugins::Options& opts)
    : PUCSFlawFinder(opts.get<int>("max_search_states"))
{
}

PUCSFlawFinder::PUCSFlawFinder(int max_search_states)
    : max_search_states_(max_search_states)
{
}

bool PUCSFlawFinder::apply_policy(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& state_ranking_function,
    const ProjectionStateSpace& mdp,
    const ProjectionMultiPolicy& policy,
    const std::unordered_set<int>& blacklisted_variables,
    std::vector<Flaw>& flaw_list,
    utils::CountdownTimer& timer)
{
    assert(pq_.empty() && probabilities_.empty());

    // Exception safety due to TimeoutException
    scope_exit guard([&] {
        pq_.clear();
        probabilities_.clear();
    });

    StateRegistry registry(task_proxy);

    {
        const State& init = registry.get_initial_state();
        pq_.push(1.0, init);
        probabilities_[StateID(init.get_id())].path_probability = 1.0;
    }

    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy goals = task_proxy.get_goals();

    do {
        timer.throw_if_expired();

        auto [path_probability, current] = pq_.pop();
        auto& info = probabilities_[StateID(current.get_id())];
        assert(!info.expanded);

        // TODO remove this once we have a real priority queue...
        if (path_probability < info.path_probability) {
            continue;
        }

        info.expanded = true;

        assert(path_probability != 0_vt);

        // Check flaws, generate successors
        const StateRank abs = state_ranking_function.get_abstract_rank(current);

        const std::vector abs_decisions = policy.get_decisions(abs);

        // We reached a terminal state, check if it is a goal or dead-end
        if (abs_decisions.empty()) {
            if (mdp.is_goal(abs) &&
                collect_flaws(goals, current, blacklisted_variables, flaw_list))
                return false;

            continue;
        }

        std::vector<Flaw> local_flaws;

        for (const auto& decision : abs_decisions) {
            const auto* abs_op = decision.action;
            const auto op = operators[abs_op->operator_id];

            if (collect_flaws(
                    op.get_preconditions(),
                    current,
                    blacklisted_variables,
                    local_flaws)) {
                continue; // Try next operator
            }

            // Generate the successors and add them to the queue
            for (const auto outcome : op.get_outcomes()) {
                State successor = registry.get_successor_state(
                    current,
                    outcome.get_effects());

                if (static_cast<int>(registry.size()) > max_search_states_) {
                    return false;
                }

                auto& succ_entry = probabilities_[StateID(successor.get_id())];
                const auto succ_prob =
                    path_probability * outcome.get_probability();

                if (!succ_entry.expanded &&
                    succ_entry.path_probability < succ_prob) {
                    succ_entry.path_probability = succ_prob;
                    pq_.push(succ_prob, successor);
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
    } while (!pq_.empty());

    return true;
}

std::string PUCSFlawFinder::get_name()
{
    return "PUCS Flaw Finder";
}

class PUCSFlawFinderFeature
    : public plugins::TypedFeature<FlawFindingStrategy, PUCSFlawFinder> {
public:
    PUCSFlawFinderFeature()
        : TypedFeature("pucs_flaw_finder")
    {
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            plugins::Bounds("0", "infinity"));
    }
};

static plugins::FeaturePlugin<PUCSFlawFinderFeature> _plugin;

} // namespace probfd::pdbs::cegar
