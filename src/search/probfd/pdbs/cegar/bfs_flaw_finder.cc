#include "probfd/pdbs/cegar/bfs_flaw_finder.h"

#include "probfd/pdbs/cegar/flaw.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/multi_policy.h"
#include "probfd/task_proxy.h"

#include "probfd/utils/guards.h"

#include "downward/utils/countdown_timer.h"

#include "downward/state_registry.h"

#include "downward/plugins/plugin.h"

#include <cassert>
#include <utility>

using namespace std;
using namespace utils;

namespace probfd::pdbs::cegar {

BFSFlawFinder::BFSFlawFinder(const plugins::Options& opts)
    : BFSFlawFinder(opts.get<int>("max_search_states"))
{
}

BFSFlawFinder::BFSFlawFinder(int max_search_states)
    : closed_(false)
    , max_search_states_(max_search_states)
{
}

bool BFSFlawFinder::apply_policy(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& state_ranking_function,
    const ProjectionStateSpace& mdp,
    const ProjectionMultiPolicy& policy,
    std::vector<Flaw>& flaws,
    const std::function<bool(const Flaw&)>& accept_flaw,
    utils::CountdownTimer& timer)
{
    assert(open_.empty() && closed_.empty());

    // Exception safety due to TimeoutException
    scope_exit guard([&] {
        open_.clear();
        closed_.clear();
    });

    StateRegistry registry(task_proxy);

    {
        const State& init = registry.get_initial_state();
        open_.push_back(init);
        closed_[init.get_id()] = true;
    }

    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy goals = task_proxy.get_goals();

    do {
        timer.throw_if_expired();

        const State& current = open_.front();
        const StateRank abs = state_ranking_function.get_abstract_rank(current);

        {
            const std::vector abs_decisions = policy.get_decisions(abs);

            // We reached a terminal state, check if it is a goal or dead-end
            if (abs_decisions.empty()) {
                if (mdp.is_goal(abs) &&
                    collect_flaws(goals, current, flaws, accept_flaw))
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
                        flaws,
                        accept_flaw)) {
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
            flaws.insert(flaws.end(), local_flaws.begin(), local_flaws.end());

            return false;
        }

    continue_exploration:;
        open_.pop_front();

    } while (!open_.empty());

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

} // namespace probfd::pdbs::cegar
