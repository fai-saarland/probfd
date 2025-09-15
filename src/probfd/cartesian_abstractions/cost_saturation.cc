#include "probfd/cartesian_abstractions/cost_saturation.h"

#include "downward/initial_state_values.h"
#include "probfd/abstractions/distances.h"

#include "probfd/cartesian_abstractions/cartesian_abstraction.h"
#include "probfd/cartesian_abstractions/cartesian_heuristic_function.h"
#include "probfd/cartesian_abstractions/cegar.h"
#include "probfd/cartesian_abstractions/heuristics.h"
#include "probfd/cartesian_abstractions/probabilistic_transition_system.h"
#include "probfd/cartesian_abstractions/subtask_generators.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/tasks/range_operator_cost_function.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/memory.h"
#include "downward/utils/timer.h"

#include "downward/state.h"

#include <cassert>
#include <ostream>
#include <ranges>
#include <utility>

using namespace std;
using namespace downward;

namespace probfd::cartesian_abstractions {

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

    const int num_states = static_cast<int>(h_values.size());

    const auto& out_transitions = transition_system.get_outgoing_transitions();
    const auto& loops = transition_system.get_loops();

    for (int state_id = 0; state_id < num_states; ++state_id) {
        value_t h = h_values[state_id];

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
    downward::utils::Duration max_time,
    bool use_general_costs,
    utils::LogProxy log)
    : subtask_generators_(subtask_generators)
    , flaw_generator_factory_(std::move(flaw_generator_factory))
    , split_selector_factory_(std::move(split_selector_factory))
    , max_states_(max_states)
    , max_non_looping_transitions_(max_non_looping_transitions)
    , max_time_(max_time)
    , use_general_costs_(use_general_costs)
    , log_(std::move(log))
    , num_abstractions_(0)
    , num_states_(0)
    , num_non_looping_transitions_(0)
{
}

CostSaturation::~CostSaturation() = default;

vector<CartesianHeuristicFunction> CostSaturation::generate_heuristic_functions(
    const SharedProbabilisticTask& task)
{
    // For simplicity this is a member object. Make sure it is in a valid state.
    assert(heuristic_functions_.empty());

    utils::CountdownTimer timer(max_time_);

    const auto& axioms = get_axioms(task);
    const auto& operators = get_operators(task);
    const auto& init_vals = get_init(task);
    const auto& cost_function = get_cost_function(task);

    ::task_properties::verify_no_axioms(axioms);
    task_properties::verify_no_conditional_effects(operators);

    reset(operators, cost_function);

    State initial_state = init_vals.get_initial_state();

    function<bool()> should_abort = [&]() {
        return num_states_ >= max_states_ ||
               num_non_looping_transitions_ >= max_non_looping_transitions_ ||
               timer.is_expired() ||
               !utils::extra_memory_padding_is_reserved() ||
               state_is_dead_end(initial_state);
    };

    utils::reserve_extra_memory_padding(memory_padding_in_mb);
    for (const shared_ptr<SubtaskGenerator>& subtask_generator :
         subtask_generators_) {
        SharedTasks subtasks = subtask_generator->get_subtasks(task, log_);
        build_abstractions(subtasks, timer, should_abort);
        if (should_abort()) break;
    }
    if (utils::extra_memory_padding_is_reserved())
        utils::release_extra_memory_padding();
    print_statistics(timer.get_elapsed_time());

    vector<CartesianHeuristicFunction> functions;
    swap(heuristic_functions_, functions);

    return functions;
}

void CostSaturation::reset(
    const ProbabilisticOperatorSpace& operators,
    const OperatorCostFunction<value_t>& cost_function)
{
    remaining_costs_ =
        task_properties::get_operator_costs(operators, cost_function);
    num_abstractions_ = 0;
    num_states_ = 0;
}

void CostSaturation::reduce_remaining_costs(
    const vector<value_t>& saturated_costs)
{
    assert(remaining_costs_.size() == saturated_costs.size());

    for (auto&& [remaining, saturated] :
         std::views::zip(remaining_costs_, saturated_costs)) {
        assert(!is_approx_greater(saturated, remaining, 0.001));
        /* Since we ignore transitions from states s with h(s)=INFINITE_VALUE,
           all saturated costs (h(s)-h(s')) are finite or -INFINITE_VALUE. */
        assert(saturated != INFINITE_VALUE);

        remaining -= saturated;
    }

    // Guard against tolerance issues
    std::ranges::replace_if(
        remaining_costs_,
        [](value_t cost) { return is_approx_equal(0_vt, cost, 0.001); },
        0_vt);
}

bool CostSaturation::state_is_dead_end(const State& state) const
{
    return std::ranges::any_of(heuristic_functions_, [&](const auto& function) {
        return function.get_value(state) == INFINITE_VALUE;
    });
}

void CostSaturation::build_abstractions(
    const SharedTasks& subtasks,
    const utils::CountdownTimer& timer,
    function<bool()> should_abort)
{
    auto cf = downward::extra_tasks::make_shared_range_cf(remaining_costs_);
    int rem_subtasks = static_cast<int>(subtasks.size());

    for (auto [subtask, state_mapping, _] : subtasks) {
        subtask = replace(subtask, cf);

        assert(num_states_ < max_states_);
        CEGAR cegar(
            max(1, (max_states_ - num_states_) / rem_subtasks),
            max(1,
                (max_non_looping_transitions_ - num_non_looping_transitions_) /
                    rem_subtasks),
            timer.get_remaining_time() / rem_subtasks,
            flaw_generator_factory_,
            split_selector_factory_,
            log_);

        auto [refinement_hierarchy, abstraction, heuristic] =
            cegar.run_refinement_loop(subtask);

        ++num_abstractions_;
        num_states_ += abstraction->get_num_states();
        num_non_looping_transitions_ +=
            abstraction->get_transition_system().get_num_non_loops();
        assert(num_states_ <= max_states_);

        vector<value_t> goal_distances(
            abstraction->get_num_states(),
            std::numeric_limits<value_t>::quiet_NaN());
        compute_value_table(
            *abstraction,
            abstraction->get_initial_state().get_id(),
            *heuristic,
            goal_distances);
        vector<value_t> saturated_costs = compute_saturated_costs(
            abstraction->get_transition_system(),
            goal_distances,
            use_general_costs_);

        heuristic_functions_.emplace_back(
            state_mapping,
            subtask,
            std::move(refinement_hierarchy),
            std::move(goal_distances));

        reduce_remaining_costs(saturated_costs);

        if (should_abort()) break;

        --rem_subtasks;
    }
}

void CostSaturation::print_statistics(utils::Duration init_time) const
{
    if (log_.is_at_least_normal()) {
        log_.println("Done initializing additive Cartesian heuristic");
        log_.println(
            "Time for initializing additive Cartesian heuristic: {}",
            init_time);
        log_.println("Cartesian abstractions built: {}", num_abstractions_);
        log_.println("Cartesian states: {}", num_states_);
        log_.println(
            "Total number of non-looping transitions: {}",
            num_non_looping_transitions_);
        log_.println();
    }
}

} // namespace probfd::cartesian_abstractions
