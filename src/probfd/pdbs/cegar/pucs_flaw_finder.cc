#include "probfd/pdbs/cegar/pucs_flaw_finder.h"

#include "probfd/pdbs/cegar/flaw.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/types.h"

#include "probfd/utils/guards.h"

#include "probfd/multi_policy.h"
#include "probfd/probabilistic_task.h"

#include "downward/utils/countdown_timer.h"

#include "downward/state_registry.h"

#include <cassert>

using namespace std;
using namespace downward;
using namespace downward::utils;

namespace probfd::pdbs::cegar {

PUCSFlawFinder::PUCSFlawFinder(int max_search_states)
    : max_search_states_(max_search_states)
{
}

bool PUCSFlawFinder::apply_policy(
    const ProbabilisticTask& task,
    const StateRankingFunction& state_ranking_function,
    const ProjectionStateSpace& mdp,
    const ProjectionMultiPolicy& policy,
    std::vector<Flaw>& flaws,
    const std::function<bool(const Flaw&)>& accept_flaw,
    utils::CountdownTimer& timer)
{
    assert(pq_.empty() && probabilities_.empty());

    // Exception safety due to TimeoutException
    scope_exit guard([&] {
        pq_.clear();
        probabilities_.clear();
    });

    StateRegistry registry(task);

    {
        const State& init = registry.get_initial_state();
        pq_.push(1.0, init);
        probabilities_[StateID(init.get_id())].path_probability = 1.0;
    }

    const ProbabilisticOperatorsProxy operators = task.get_operators();
    const GoalsProxy goals = task.get_goals();

    do {
        timer.throw_if_expired();

        auto [path_probability, current] = pq_.pop();
        auto& info = probabilities_[StateID(current.get_id())];
        assert(!info.expanded);

        // TODO remove this once we have a real priority queue...
        if (path_probability < info.path_probability) { continue; }

        info.expanded = true;

        assert(path_probability != 0_vt);

        // Check flaws, generate successors
        const StateRank abs = state_ranking_function.get_abstract_rank(current);

        const auto abs_decisions = policy.get_decisions(abs);

        // We reached a terminal state, check if it is a goal or dead-end
        if (abs_decisions.empty()) {
            if (mdp.is_goal(abs) &&
                collect_flaws(goals, current, flaws, accept_flaw))
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
                    local_flaws,
                    accept_flaw)) {
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
        flaws.insert(flaws.end(), local_flaws.begin(), local_flaws.end());

        return false;

    continue_exploration:;
    } while (!pq_.empty());

    return true;
}

std::string PUCSFlawFinder::get_name()
{
    return "PUCS Flaw Finder";
}

} // namespace probfd::pdbs::cegar
