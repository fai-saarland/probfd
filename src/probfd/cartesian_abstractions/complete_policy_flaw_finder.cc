#include "probfd/cartesian_abstractions/complete_policy_flaw_finder.h"

#include "downward/initial_state_values.h"
#include "probfd/cartesian_abstractions/abstract_state.h"
#include "probfd/cartesian_abstractions/cartesian_abstraction.h"
#include "probfd/cartesian_abstractions/probabilistic_transition.h"
#include "probfd/cartesian_abstractions/utils.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/policy.h"
#include "probfd/probabilistic_task.h"

#include "downward/state_registry.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/memory.h"

#include "downward/state.h"
#include "downward/state_id.h"
#include "probfd/probabilistic_operator_space.h"

#include <cassert>
#include <deque>
#include <ostream>
#include <ranges>
#include <utility>

using namespace std;
using namespace std::views;
using namespace downward;

namespace probfd::cartesian_abstractions {

CompletePolicyFlawFinder::CompletePolicyFlawFinder(int max_search_states)
    : max_search_states_(max_search_states)
{
}

optional<Flaw> CompletePolicyFlawFinder::find_flaw(
    const ProbabilisticTaskTuple& task,
    const std::vector<int>& domain_sizes,
    CartesianAbstraction& abstraction,
    Solution& policy,
    utils::LogProxy& log,
    utils::CountdownTimer& timer)
{
    const auto& variables = get_variables(task);
    const auto& axioms = get_axioms(task);
    const auto& operators = get_operators(task);
    const auto& goals = get_goal(task);
    const auto& init_vals = get_init(task);

    StateRegistry registry(variables, axioms, init_vals.get_initial_state());

    struct QueueItem {
        downward::StateID state_id;
        const AbstractState* abstract_state;
    };

    const State initial = registry.get_initial_state();
    const AbstractState* abstract_initial = &abstraction.get_initial_state();

    std::deque<QueueItem> frontier({{initial.get_id(), abstract_initial}});
    storage::PerStateStorage<bool> visited(false);
    visited[0] = true;

    for (; !frontier.empty(); frontier.pop_front()) {
        timer.throw_if_expired();
        if (!utils::extra_memory_padding_is_reserved()) return std::nullopt;

        auto& [state_id, abstract_state] = frontier.front();

        State state = registry.lookup_state(state_id);

        auto decision = policy.get_decision(abstract_state->get_id());

        // Check for goal state
        if (!decision) {
            assert(abstraction.get_goals().contains(abstract_state->get_id()));

            if (!downward::task_properties::is_goal_state(goals, state)) {
                if (log.is_at_least_debug()) log << "Goal test failed." << endl;
                state.unpack();
                return Flaw(
                    std::move(state),
                    *abstract_state,
                    get_cartesian_set(domain_sizes, goals));
            }
            continue;
        }

        const ProbabilisticTransition* transition = decision->action;
        const auto op = operators[transition->op_id];

        // Check for operator applicability
        if (!task_properties::is_applicable(op, state)) {
            if (log.is_at_least_debug())
                log << "Operator not applicable: " << op.get_name() << endl;
            state.unpack();
            return Flaw(
                std::move(state),
                *abstract_state,
                get_cartesian_set(domain_sizes, op.get_preconditions()));
        }

        const auto& targets = transition->target_ids;

        // Generate successors and check for matching abstract states
        for (const auto [outcome, abs_t] : zip(op.get_outcomes(), targets)) {
            State next_concrete =
                registry.get_successor_state(state, outcome.get_effects());

            if (static_cast<int>(registry.size()) > max_search_states_) {
                if (log.is_at_least_normal()) {
                    log << "Reached maximal number of flaw search states."
                        << endl;
                }
                return std::nullopt;
            }

            const auto* next_abstract = &abstraction.get_abstract_state(abs_t);

            if (!next_abstract->includes(next_concrete)) {
                if (log.is_at_least_debug()) log << "  Paths deviate." << endl;
                state.unpack();
                return Flaw(
                    std::move(state),
                    *abstract_state,
                    next_abstract->regress(op, outcome.get_effects()));
            }

            // Add successor to frontier if not seen before
            auto bool_proxy = visited[next_concrete.get_id()];
            if (!bool_proxy) {
                bool_proxy = true;
                frontier.emplace_back(next_concrete.get_id(), next_abstract);
            }
        }
    }

    // We found a concrete policy.
    if (log.is_at_least_normal()) {
        log << "Found concrete solution during refinement." << endl;
    }

    return std::nullopt;
}

} // namespace probfd::cartesian_abstractions
