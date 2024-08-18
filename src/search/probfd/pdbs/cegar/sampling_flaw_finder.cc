#include "probfd/pdbs/cegar/sampling_flaw_finder.h"

#include "probfd/pdbs/cegar/flaw.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/types.h"

#include "probfd/utils/guards.h"

#include "probfd/distribution.h"
#include "probfd/multi_policy.h"
#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"

#include "downward/state_registry.h"

#include <cassert>
#include <utility>

using namespace std;
using namespace utils;

namespace probfd::pdbs::cegar {

SamplingFlawFinder::SamplingFlawFinder(
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    int max_search_states)
    : rng_(std::move(rng))
    , max_search_states_(max_search_states)
{
}

SamplingFlawFinder::~SamplingFlawFinder() = default;

bool SamplingFlawFinder::apply_policy(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& state_ranking_function,
    const ProjectionStateSpace& mdp,
    const ProjectionMultiPolicy& policy,
    std::vector<Flaw>& flaws,
    const std::function<bool(const Flaw&)>& accept_flaw,
    utils::CountdownTimer& timer)
{
    assert(stk_.empty() && einfos_.empty());

    // Exception safety due to TimeoutException
    scope_exit guard([&] {
        stk_.clear();
        einfos_.clear();
    });

    StateRegistry registry(task_proxy);
    stk_.push_back(registry.get_initial_state());

    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy goals = task_proxy.get_goals();

    for (;;) {
        const State* current = &stk_.back();
        const StateRank abs =
            state_ranking_function.get_abstract_rank(*current);

        ExplorationInfo* einfo = &einfos_[StateID(current->get_id())];

        assert(!einfo->explored);
        einfo->explored = true;

        {
            const std::vector abs_decisions = policy.get_decisions(abs);

            // Goal flaw check
            if (abs_decisions.empty()) {
                if (mdp.is_goal(abs) &&
                    collect_flaws(goals, *current, flaws, accept_flaw)) {
                    return false;
                }

                goto backtrack;
            }

            std::vector<Flaw> local_flaws;

            // Precondition flaw check
            for (const auto& decision : abs_decisions) {
                const auto* abs_op = decision.action;
                const auto op = operators[abs_op->operator_id];

                if (collect_flaws(
                        op.get_preconditions(),
                        *current,
                        local_flaws,
                        accept_flaw)) {
                    continue; // Try next operator
                }

                // Generate the successors
                for (const auto outcome : op.get_outcomes()) {
                    const State successor = registry.get_successor_state(
                        *current,
                        outcome.get_effects());

                    if (static_cast<int>(registry.size()) >
                        max_search_states_) {
                        return false;
                    }

                    einfo->successors.add_probability(
                        successor.get_id(),
                        outcome.get_probability());
                }

                goto operator_found;
            }

            // Insert all flaws of all operators
            flaws.insert(flaws.end(), local_flaws.begin(), local_flaws.end());

            return false;
        }

    operator_found:;

        for (;;) {
            while (!einfo->successors.empty()) {
                timer.throw_if_expired();

                // Sample next successor
                auto it = einfo->successors.sample(*rng_);
                const StateID succ_id = it->item;
                einfo->successors.erase(it);

                State successor = registry.lookup_state(succ_id);

                // Ignore states already seen
                if (!einfos_[succ_id].explored) {
                    stk_.push_back(std::move(successor));
                    goto continue_exploration;
                }
            }

        backtrack:;

            do {
                stk_.pop_back();

                if (stk_.empty()) {
                    return true;
                }

                current = &stk_.back();
                einfo = &einfos_[StateID(current->get_id())];
            } while (einfo->successors.empty());
        }

    continue_exploration:;
    }
}

std::string SamplingFlawFinder::get_name()
{
    return "Sampling Flaw Finder";
}

} // namespace probfd::pdbs::cegar
