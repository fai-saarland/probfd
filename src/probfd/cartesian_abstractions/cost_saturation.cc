#include "probfd/cartesian_abstractions/cost_saturation.h"

#include "probfd/abstractions/distances.h"

#include "probfd/cartesian_abstractions/cartesian_abstraction.h"
#include "probfd/cartesian_abstractions/cartesian_heuristic_function.h"
#include "probfd/cartesian_abstractions/cegar.h"
#include "probfd/cartesian_abstractions/heuristics.h"
#include "probfd/cartesian_abstractions/probabilistic_transition_system.h"
#include "probfd/cartesian_abstractions/subtask_generators.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/tasks/range_operator_cost_function.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/memory.h"
#include "downward/utils/timer.h"

#include "downward/initial_state_values.h"
#include "downward/state.h"
#include "probfd/cartesian_abstractions/flaw_generator.h"
#include "probfd/cartesian_abstractions/split_selector.h"

#include <cassert>
#include <ostream>
#include <ranges>
#include <utility>

using namespace std;
using namespace downward;

namespace probfd::cartesian_abstractions {

namespace {

/*
  We reserve some memory to be able to recover from out-of-memory
  situations gracefully. When the memory runs out, we stop refining and
  start the next refinement or the search. Due to memory fragmentation
  the memory used for building the abstraction (states, transitions,
  etc.) often can't be reused for things that require big continuous
  blocks of memory. It is for this reason that we require such a large
  amount of memory padding.
*/
constexpr int memory_padding_in_mb = 75;

void reduce_remaining_costs(
    std::vector<value_t>& remaining_costs,
    const vector<value_t>& saturated_costs)
{
    assert(remaining_costs.size() == saturated_costs.size());

    for (auto&& [remaining, saturated] :
         std::views::zip(remaining_costs, saturated_costs)) {
        assert(!is_approx_greater(saturated, remaining, 0.001));
        /* Since we ignore transitions from states s with h(s)=INFINITE_VALUE,
           all saturated costs (h(s)-h(s')) are finite or -INFINITE_VALUE. */
        assert(saturated != INFINITE_VALUE);

        remaining -= saturated;
    }

    // Guard against tolerance issues
    std::ranges::replace_if(
        remaining_costs,
        [](value_t cost) { return is_approx_equal(0_vt, cost, 0.001); },
        0_vt);
}

vector<value_t> compute_saturated_costs(
    const ProbabilisticTransitionSystem& transition_system,
    const vector<value_t>& h_values,
    bool use_general_costs)
{
    vector saturated_costs(
        transition_system.get_num_operators(),
        use_general_costs ? -INFINITE_VALUE : 0);

    const int num_states = static_cast<int>(h_values.size());

    const auto& out_transitions = transition_system.get_outgoing_transitions();
    const auto& loops = transition_system.get_loops();

    for (int state_id = 0; state_id < num_states; ++state_id) {
        const value_t h = h_values[state_id];

        /*
          No need to maintain goal distances of unreachable and dead end
          states (h == INFINITE_VALUE).
        */
        if (h == INFINITE_VALUE) continue;

        for (const auto* transition : out_transitions[state_id]) {
            const int op_id = transition->op_id;

            value_t expectation = 0_vt;

            for (size_t i = 0; i != transition->target_ids.size(); ++i) {
                const int succ_id = transition->target_ids[i];
                const value_t succ_h = h_values[succ_id];
                if (succ_h == INFINITE_VALUE) goto next_transition;
                const value_t probability =
                    transition_system.get_probability(op_id, i);
                expectation += probability * succ_h;
            }

            saturated_costs[op_id] =
                max(saturated_costs[op_id], h - expectation);

        next_transition:;
        }

        if (use_general_costs) {
            /* To prevent negative cost cycles, all operators inducing
               self-loops must have non-negative costs. */
            for (const int op_id : loops[state_id]) {
                saturated_costs[op_id] = max(saturated_costs[op_id], 0_vt);
            }
        }
    }
    return saturated_costs;
}

} // namespace

vector<CartesianHeuristicFunction> generate_heuristic_functions(
    const std::vector<std::shared_ptr<SubtaskGenerator>>& subtask_generators,
    FlawGeneratorFactory& flaw_generator_factory,
    SplitSelectorFactory& split_selector_factory,
    int max_states,
    int max_non_looping_transitions,
    utils::FSeconds max_time,
    bool use_general_costs,
    utils::LogProxy& log,
    const SharedProbabilisticTask& task)
{
    const utils::CountdownTimer timer(max_time);

    const auto& axioms = get_axioms(task);
    const auto& operators = get_operators(task);
    const auto& init_vals = get_init(task);
    const auto& cost_function = get_cost_function(task);

    ::task_properties::verify_no_axioms(axioms);
    task_properties::verify_no_conditional_effects(operators);

    std::vector<CartesianHeuristicFunction> heuristic_functions;

    std::vector remaining_costs =
        task_properties::get_operator_costs(operators, cost_function);

    int num_states = 0;
    int num_non_looping_transitions = 0;

    const State initial_state = init_vals.get_initial_state();

    const function<bool()> should_abort = [&] {
        return num_states >= max_states ||
               num_non_looping_transitions >= max_non_looping_transitions ||
               timer.is_expired() ||
               !utils::extra_memory_padding_is_reserved() ||
               std::ranges::any_of(
                   heuristic_functions,
                   [&](const auto& function) {
                       return function.get_value(initial_state) ==
                              INFINITE_VALUE;
                   });
    };

    utils::reserve_extra_memory_padding(memory_padding_in_mb);

    for (const auto& subtask_generator : subtask_generators) {
        auto cf = downward::extra_tasks::make_shared_range_cf(remaining_costs);

        SharedTasks subtasks = subtask_generator->get_subtasks(task, log);
        int rem_subtasks = static_cast<int>(subtasks.size());

        for (auto [subtask, state_mapping, _] : subtasks) {
            subtask = replace(subtask, cf);

            const auto flaw_generator =
                flaw_generator_factory.create_flaw_generator();

            const auto split_selector =
                split_selector_factory.create_split_selector(subtask);

            assert(num_states < max_states_);
            auto [refinement_hierarchy, abstraction, heuristic] =
                run_refinement_loop(
                    max(1, (max_states - num_states) / rem_subtasks),
                    max(1,
                        (max_non_looping_transitions -
                         num_non_looping_transitions) /
                            rem_subtasks),
                    timer.get_remaining_time() / rem_subtasks,
                    *flaw_generator,
                    *split_selector,
                    log,
                    to_refs(subtask));

            num_states += abstraction->get_num_states();
            num_non_looping_transitions +=
                abstraction->get_transition_system().get_num_non_loops();
            assert(num_states <= max_states_);

            // Compute goal distances.
            vector goal_distances(
                abstraction->get_num_states(),
                std::numeric_limits<value_t>::quiet_NaN());

            compute_value_table(
                *abstraction,
                abstraction->get_initial_state().get_id(),
                *heuristic,
                goal_distances);

            // Compute saturated and remaining costs.
            vector<value_t> saturated_costs = compute_saturated_costs(
                abstraction->get_transition_system(),
                goal_distances,
                use_general_costs);

            reduce_remaining_costs(remaining_costs, saturated_costs);

            // Add heuristic function.
            heuristic_functions.emplace_back(
                state_mapping,
                std::move(refinement_hierarchy),
                std::move(goal_distances));

            --rem_subtasks;

            if (should_abort()) goto break_outer_loop;
        }

        if (should_abort()) goto break_outer_loop;
    }

break_outer_loop:;

    if (utils::extra_memory_padding_is_reserved())
        utils::release_extra_memory_padding();

    auto time = timer.get_elapsed_time();

    if (log.is_at_least_normal()) {
        log.println("Done initializing additive Cartesian heuristic");
        log.println(
            "Time for initializing additive Cartesian heuristic: {}",
            time);
        log.println(
            "Cartesian abstractions built: {}",
            heuristic_functions.size());
        log.println("Cartesian states: {}", num_states);
        log.println(
            "Total number of non-looping transitions: {}",
            num_non_looping_transitions);
        log.println();
    }

    return heuristic_functions;
}

} // namespace probfd::cartesian_abstractions
