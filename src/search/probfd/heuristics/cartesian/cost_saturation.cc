#include "probfd/heuristics/cartesian/cost_saturation.h"

#include "probfd/heuristics/cartesian/abstraction.h"
#include "probfd/heuristics/cartesian/cartesian_abstraction_info.h"
#include "probfd/heuristics/cartesian/cartesian_heuristic_function.h"
#include "probfd/heuristics/cartesian/cegar.h"
#include "probfd/heuristics/cartesian/distances.h"
#include "probfd/heuristics/cartesian/flaw_generator.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/split_selector.h"
#include "probfd/heuristics/cartesian/subtask_generators.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/task_utils/task_properties.h"
#include "probfd/tasks/modified_operator_costs_task.h"

#include "downward/cartesian_abstractions/abstract_state.h"
#include "downward/cartesian_abstractions/cartesian_heuristic_function.h"
#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/memory.h"

#include <algorithm>
#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

/*
  We reserve some memory to be able to recover from out-of-memory
  situations gracefully. When the memory runs out, we stop refining and
  start the next refinement or the search. Due to memory fragmentation
  the memory used for building the abstraction (states, transitions,
  etc.) often can't be reused for things that require big continuous
  blocks of memory. It is for this reason that we require such a large
  amount of memory padding.
*/
static const int memory_padding_in_mb = 75;

static vector<value_t> compute_saturated_costs(
    const ProbabilisticTransitionSystem& transition_system,
    const vector<value_t>& h_values,
    bool use_general_costs)
{
    vector<value_t> saturated_costs(
        transition_system.get_num_operators(),
        use_general_costs ? -INFINITE_VALUE : 0);

    const int num_states = h_values.size();

    const auto& out_transitions = transition_system.get_outgoing_transitions();
    const auto& partial_loops = transition_system.get_partial_loops();
    const auto& loops = transition_system.get_loops();

    for (int state_id = 0; state_id < num_states; ++state_id) {
        value_t h = h_values[state_id];

        /*
          No need to maintain goal distances of unreachable and dead end
          states (h == INFINITE_VALUE).
        */
        if (h == INFINITE_VALUE) continue;

        for (const auto& transition : out_transitions[state_id]) {
            const int op_id = transition.op_id;

            value_t expectation = 0_vt;

            for (size_t i = 0; i != transition.target_ids.size(); ++i) {
                const int succ_id = transition.target_ids[i];
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

        for (const auto& transition : partial_loops[state_id]) {
            const int op_id = transition.op_id;

            value_t expectation = 0_vt;

            for (size_t i = 0; i != transition.target_ids.size(); ++i) {
                const int succ_id = transition.target_ids[i];
                const value_t succ_h = h_values[succ_id];
                if (succ_h == INFINITE_VALUE) goto next_transition2;
                const value_t probability =
                    transition_system.get_probability(op_id, i);
                expectation += probability * succ_h;
            }

            saturated_costs[op_id] =
                max(saturated_costs[op_id], h - expectation);

        next_transition2:;
        }

        if (use_general_costs) {
            /* To prevent negative cost cycles, all operators inducing
               self-loops must have non-negative costs. */
            for (int op_id : loops[state_id]) {
                saturated_costs[op_id] = max(saturated_costs[op_id], 0_vt);
            }
        }
    }
    return saturated_costs;
}

CostSaturation::CostSaturation(
    const vector<shared_ptr<SubtaskGenerator>>& subtask_generators,
    shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
    shared_ptr<SplitSelectorFactory> split_selector_factory,
    int max_states,
    int max_non_looping_transitions,
    double max_time,
    bool use_general_costs,
    utils::LogProxy log)
    : subtask_generators(subtask_generators)
    , flaw_generator_factory(flaw_generator_factory)
    , split_selector_factory(split_selector_factory)
    , max_states(max_states)
    , max_non_looping_transitions(max_non_looping_transitions)
    , max_time(max_time)
    , use_general_costs(use_general_costs)
    , log(std::move(log))
    , num_abstractions(0)
    , num_states(0)
    , num_non_looping_transitions(0)
{
}

vector<CartesianHeuristicFunction> CostSaturation::generate_heuristic_functions(
    const shared_ptr<ProbabilisticTask>& task)
{
    // For simplicity this is a member object. Make sure it is in a valid state.
    assert(heuristic_functions.empty());

    utils::CountdownTimer timer(max_time);

    ProbabilisticTaskProxy task_proxy(*task);

    ::task_properties::verify_no_axioms(task_proxy);
    task_properties::verify_no_conditional_effects(task_proxy);

    reset(task_proxy);

    State initial_state = ProbabilisticTaskProxy(*task).get_initial_state();

    function<bool()> should_abort = [&]() {
        return num_states >= max_states ||
               num_non_looping_transitions >= max_non_looping_transitions ||
               timer.is_expired() ||
               !utils::extra_memory_padding_is_reserved() ||
               state_is_dead_end(initial_state);
    };

    utils::reserve_extra_memory_padding(memory_padding_in_mb);
    for (const shared_ptr<SubtaskGenerator>& subtask_generator :
         subtask_generators) {
        SharedTasks subtasks = subtask_generator->get_subtasks(task, log);
        build_abstractions(subtasks, timer, should_abort);
        if (should_abort()) break;
    }
    if (utils::extra_memory_padding_is_reserved())
        utils::release_extra_memory_padding();
    print_statistics(timer.get_elapsed_time());

    vector<CartesianHeuristicFunction> functions;
    swap(heuristic_functions, functions);

    return functions;
}

void CostSaturation::reset(const ProbabilisticTaskProxy& task_proxy)
{
    remaining_costs = task_properties::get_operator_costs(task_proxy);
    num_abstractions = 0;
    num_states = 0;
}

void CostSaturation::reduce_remaining_costs(
    const vector<value_t>& saturated_costs)
{
    assert(remaining_costs.size() == saturated_costs.size());
    for (size_t i = 0; i < remaining_costs.size(); ++i) {
        value_t& remaining = remaining_costs[i];
        const value_t& saturated = saturated_costs[i];
        assert(!is_approx_greater(saturated, remaining, 0.001));
        /* Since we ignore transitions from states s with h(s)=INFINITE_VALUE,
           all saturated costs (h(s)-h(s')) are finite or -INFINITE_VALUE. */
        assert(saturated != INFINITE_VALUE);

        remaining -= saturated;

        // Remaining costs can become negative due to floating point imprecision
        if (remaining < 0.0_vt) {
            is_approx_equal(remaining, 0.0_vt, 0.001);
            remaining = 0.0_vt;
        }
        assert(remaining >= 0);
    }
}

shared_ptr<ProbabilisticTask> CostSaturation::get_remaining_costs_task(
    shared_ptr<ProbabilisticTask>& parent) const
{
    vector<value_t> costs = remaining_costs;
    return make_shared<extra_tasks::ModifiedOperatorCostsTask>(
        parent,
        std::move(costs));
}

bool CostSaturation::state_is_dead_end(const State& state) const
{
    for (const CartesianHeuristicFunction& function : heuristic_functions) {
        if (function.get_value(state) == INFINITE_VALUE) return true;
    }
    return false;
}

void CostSaturation::build_abstractions(
    const vector<shared_ptr<ProbabilisticTask>>& subtasks,
    const utils::CountdownTimer& timer,
    function<bool()> should_abort)
{
    int rem_subtasks = subtasks.size();
    for (shared_ptr<ProbabilisticTask> subtask : subtasks) {
        subtask = get_remaining_costs_task(subtask);

        assert(num_states < max_states);
        CEGAR cegar(
            max(1, (max_states - num_states) / rem_subtasks),
            max(1,
                (max_non_looping_transitions - num_non_looping_transitions) /
                    rem_subtasks),
            flaw_generator_factory,
            split_selector_factory,
            log);

        auto [refinement_hierarchy, abstraction, heuristic] =
            cegar.run_refinement_loop(
                subtask,
                timer.get_remaining_time() / rem_subtasks);

        ++num_abstractions;
        num_states += abstraction->get_num_states();
        num_non_looping_transitions +=
            abstraction->get_transition_system().get_num_non_loops();
        assert(num_states <= max_states);

        vector<value_t> goal_distances =
            compute_distances(*abstraction, *heuristic);
        vector<value_t> saturated_costs = compute_saturated_costs(
            abstraction->get_transition_system(),
            goal_distances,
            use_general_costs);

        heuristic_functions.emplace_back(
            std::move(refinement_hierarchy),
            std::move(goal_distances));

        reduce_remaining_costs(saturated_costs);

        if (should_abort()) break;

        --rem_subtasks;
    }
}

void CostSaturation::print_statistics(utils::Duration init_time) const
{
    if (log.is_at_least_normal()) {
        log << "Done initializing additive Cartesian heuristic" << endl;
        log << "Time for initializing additive Cartesian heuristic: "
            << init_time << endl;
        log << "Cartesian abstractions built: " << num_abstractions << endl;
        log << "Cartesian states: " << num_states << endl;
        log << "Total number of non-looping transitions: "
            << num_non_looping_transitions << endl;
        log << endl;
    }
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd
