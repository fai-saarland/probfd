#include "probfd/heuristics/cartesian/complete_policy_flaw_finder.h"

#include "probfd/heuristics/cartesian/engine_interfaces.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/split_selector.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/quotients/engine_interfaces.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/policy.h"

#include "downward/state_registry.h"

#include "downward/cegar/abstract_state.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/memory.h"

#include <cassert>
#include <deque>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

CompletePolicyFlawFinder::CompletePolicyFlawFinder(int max_search_states)
    : max_search_states(max_search_states)
{
}

optional<Flaw> CompletePolicyFlawFinder::find_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    Abstraction& abstraction,
    Solution& policy,
    utils::CountdownTimer& timer,
    utils::LogProxy& log,
    const std::vector<int>& domain_sizes)
{
    StateRegistry state_registry(task_proxy);

    struct QueueItem {
        ::StateID state_id;
        const AbstractState* abstract_state;
    };

    const State initial = state_registry.get_initial_state();
    const AbstractState* abstract_initial = &abstraction.get_initial_state();

    std::deque<QueueItem> frontier({{initial.get_id(), abstract_initial}});
    storage::PerStateStorage<bool> visited(false);
    visited[0] = true;

    for (; !frontier.empty(); frontier.pop_front()) {
        timer.throw_if_expired();
        if (!utils::extra_memory_padding_is_reserved()) return std::nullopt;

        QueueItem& next = frontier.front();

        State state = state_registry.lookup_state(next.state_id);

        auto decision = policy.get_decision(next.abstract_state);

        // Check for goal state
        if (!decision) {
            assert(abstraction.get_goals().contains(
                next.abstract_state->get_id()));

            if (!::task_properties::is_goal_state(task_proxy, state)) {
                if (log.is_at_least_debug()) log << "Goal test failed." << endl;
                state.unpack();
                return Flaw(
                    std::move(state),
                    *next.abstract_state,
                    get_cartesian_set(domain_sizes, task_proxy.get_goals()));
            }
            continue;
        }

        const ProbabilisticTransition* transition = decision->action;
        const auto op = task_proxy.get_operators()[transition->op_id];

        // Check for operator applicability
        if (!::task_properties::is_applicable(op, state)) {
            if (log.is_at_least_debug())
                log << "Operator not applicable: " << op.get_name() << endl;
            state.unpack();
            return Flaw(
                std::move(state),
                *next.abstract_state,
                get_cartesian_set(domain_sizes, op.get_preconditions()));
        }

        // Generate successors and check for matching abstract states
        for (size_t i = 0; i != transition->target_ids.size(); ++i) {
            const auto outcome = op.get_outcomes()[i];
            const AbstractState* next_abstract_state =
                &abstraction.get_abstract_state(transition->target_ids[i]);

            State next_concrete_state =
                state_registry.get_successor_state(state, outcome);

            if (static_cast<int>(state_registry.size()) > max_search_states) {
                if (log.is_at_least_normal()) {
                    log << "Reached maximal number of flaw search states."
                        << endl;
                }
                return std::nullopt;
            }

            if (!next_abstract_state->includes(next_concrete_state)) {
                if (log.is_at_least_debug()) log << "  Paths deviate." << endl;
                state.unpack();
                return Flaw(
                    std::move(state),
                    *next.abstract_state,
                    next_abstract_state->regress(op, outcome.get_effects()));
            }

            // Add successor to frontier if not seen before
            auto bool_proxy = visited[next_concrete_state.get_id()];
            if (!bool_proxy) {
                bool_proxy = true;
                frontier.emplace_back(
                    next_concrete_state.get_id(),
                    next_abstract_state);
            }
        }
    }

    // We found a concrete policy.
    if (log.is_at_least_normal()) {
        log << "Found concrete solution during refinement." << endl;
    }

    return std::nullopt;
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd