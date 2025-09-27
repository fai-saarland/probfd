#include "probfd/pdbs/cegar/bfs_flaw_finder.h"

#include "probfd/pdbs/cegar/flaw.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/multi_policy.h"
#include "probfd/probabilistic_task.h"

#include "probfd/utils/guards.h"

#include "downward/utils/countdown_timer.h"

#include "downward/state_registry.h"
#include "probfd/probabilistic_operator_space.h"

#include <cassert>
#include <utility>

using namespace std;

using namespace downward;
using namespace downward::utils;

namespace probfd::pdbs::cegar {

BFSFlawFinder::BFSFlawFinder(int max_search_states)
    : closed_(false)
    , max_search_states_(max_search_states)
{
}

bool BFSFlawFinder::apply_policy(
    const ProbabilisticTaskTuple& task,
    const downward::State& initial_state,
    const StateRankingFunction& state_ranking_function,
    const ProjectionStateSpace& mdp,
    const ProjectionMultiPolicy& policy,
    std::vector<Flaw>& flaws,
    const std::function<bool(const Flaw&)>& accept_flaw,
    CountdownTimer& timer)
{
    assert(open_.empty() && closed_.empty());

    const auto& variables = get_variables(task);
    const auto& axioms = get_axioms(task);
    const auto& operators = get_operators(task);
    const auto& goals = get_goal(task);

    // Exception safety due to TimeoutException
    scope_exit guard([&] {
        open_.clear();
        closed_.clear();
    });

    StateRegistry registry(
        downward::task_properties::g_state_packers[variables],
        g_axiom_evaluators[variables, axioms],
        initial_state);

    {
        const State& init = registry.get_initial_state();
        open_.push_back(init);
        closed_[init.get_id()] = true;
    }

    bool any_flaw_suppressed = false;

    do {
        timer.throw_if_expired();

        {
            const State& current = open_.front();
            const StateRank abs =
                state_ranking_function.get_abstract_rank(current);
            const auto abs_decisions = policy.get_decisions(abs);

            // We reached a terminal state, check if it is a goal or dead-end
            if (abs_decisions.empty()) {
                if (mdp.is_goal(abs) &&
                    collect_flaws(goals, current, flaws, accept_flaw))
                    return false;

                goto continue_exploration;
            }

            std::vector<Flaw> local_flaws;

            for (const auto& decision : abs_decisions) {
                const auto op = operators[decision.action->operator_id];

                const auto s = local_flaws.size();

                const bool flaw_suppressed = collect_flaws(
                    op.get_preconditions(),
                    current,
                    local_flaws,
                    accept_flaw);

                if (flaw_suppressed) { any_flaw_suppressed = true; }

                // was a flaw added?
                if (s != local_flaws.size()) {
                    continue; // Try next operator
                }

                // Generate the successors and add them to the open list
                for (const auto outcome : op.get_outcomes()) {
                    State successor = registry.get_successor_state(
                        current,
                        outcome.get_effects());

                    if (static_cast<int>(registry.size()) >
                        max_search_states_) {
                        return false;
                    }

                    auto seen = closed_[StateID(successor.get_id())];

                    if (!seen) {
                        seen = true;
                        open_.push_back(std::move(successor));
                    }
                }

                goto continue_exploration;
            }

            // Insert all flaws of all operators
            flaws.append_range(local_flaws);

            return false;
        }

    continue_exploration:;
        open_.pop_front();

    } while (!open_.empty());

    return !any_flaw_suppressed;
}

std::string BFSFlawFinder::get_name()
{
    return "BFS Flaw Finder";
}

} // namespace probfd::pdbs::cegar
