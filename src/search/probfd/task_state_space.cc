#include "probfd/task_state_space.h"
#include "probfd/task_proxy.h"

#include "downward/algorithms/int_packer.h"

#include "downward/evaluator.h"

#include "downward/tasks/root_task.h"

#include <algorithm>
#include <cstring>
#include <map>

namespace probfd {

void InducedTaskStateSpace::Statistics::print(utils::LogProxy log) const
{
    log << "  Applicable operators: " << generated_operators << " generated, "
        << computed_operators << " computed, " << aops_generator_calls
        << " generator calls." << std::endl;
    log << "  Generated " << generated_states
        << " successor state(s): " << computed_successors << " computed, "
        << single_transition_generator_calls << " single-transition calls, "
        << all_transitions_generator_calls << " all-transitions calls."
        << std::endl;
}

InducedTaskStateSpace::InducedTaskStateSpace(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log,
    TaskSimpleCostFunction* cost_function,
    const std::vector<std::shared_ptr<::Evaluator>>& path_dependent_evaluators)
    : task_proxy(*task)
    , log(std::move(log))
    , gen_(task_proxy)
    , state_registry_(task_proxy)
    , cost_function_(cost_function)
    , notify_(path_dependent_evaluators)
{
}

StateID InducedTaskStateSpace::get_state_id(const State& state)
{
    return state.get_id();
}

State InducedTaskStateSpace::get_state(StateID state_id)
{
    return state_registry_.lookup_state(::StateID(state_id));
}

void InducedTaskStateSpace::generate_applicable_actions(
    StateID state_id,
    std::vector<OperatorID>& result)
{
    State state = state_registry_.lookup_state(::StateID(state_id));
    compute_applicable_operators(state, result);

    ++statistics_.aops_generator_calls;
    statistics_.generated_operators += result.size();
}

void InducedTaskStateSpace::generate_action_transitions(
    StateID state_id,
    OperatorID op_id,
    Distribution<StateID>& result)
{
    State state = state_registry_.lookup_state(::StateID(state_id));
    compute_successor_dist(state, op_id, result);

    ++statistics_.single_transition_generator_calls;
}

void InducedTaskStateSpace::generate_all_transitions(
    StateID state_id,
    std::vector<OperatorID>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    State state = state_registry_.lookup_state(::StateID(state_id));
    compute_applicable_operators(state, aops);
    successors.reserve(aops.size());

    for (const auto& op : aops) {
        compute_successor_dist(state, op, successors.emplace_back());
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += aops.size();
}

void InducedTaskStateSpace::generate_all_transitions(
    StateID state_id,
    std::vector<Transition>& transitions)
{
    State state = state_registry_.lookup_state(::StateID(state_id));
    gen_.generate_transitions(state, transitions, *this);

    ++statistics_.aops_computations;
    ++statistics_.all_transitions_generator_calls;    
    statistics_.computed_operators += transitions.size();
    statistics_.generated_operators += transitions.size();
}

value_t InducedTaskStateSpace::get_action_cost(OperatorID op)
{
    return cost_function_->get_action_cost(op);
}

bool InducedTaskStateSpace::is_goal(param_type<State> state) const
{
    return cost_function_->is_goal(state);
}

value_t InducedTaskStateSpace::get_goal_termination_cost() const
{
    return cost_function_->get_goal_termination_cost();
}

value_t InducedTaskStateSpace::get_non_goal_termination_cost() const
{
    return cost_function_->get_non_goal_termination_cost();
}

const State& InducedTaskStateSpace::get_initial_state()
{
    return state_registry_.get_initial_state();
}

size_t InducedTaskStateSpace::get_num_registered_states() const
{
    return state_registry_.size();
}

void InducedTaskStateSpace::print_statistics() const
{
    statistics_.print(log);
}

void InducedTaskStateSpace::compute_successor_dist(
    const State& state,
    OperatorID op_id,
    Distribution<StateID>& successor_dist)
{
    const ProbabilisticOperatorProxy op = task_proxy.get_operators()[op_id];
    const auto outcomes = op.get_outcomes();
    const size_t num_outcomes = outcomes.size();
    successor_dist.reserve(num_outcomes);

    for (const ProbabilisticOutcomeProxy outcome : outcomes) {
        value_t probability = outcome.get_probability();
        State succ = state_registry_.get_successor_state(state, outcome);

        for (const auto& h : notify_) {
            OperatorID det_op_id(outcome.get_determinization_id());
            h->notify_state_transition(state, det_op_id, succ);
        }

        successor_dist.add_probability(succ.get_id(), probability);
    }

    ++statistics_.transition_computations;
    statistics_.computed_successors += num_outcomes;
    statistics_.generated_states += successor_dist.size();
}

void InducedTaskStateSpace::compute_applicable_operators(
    const State& s,
    std::vector<OperatorID>& ops)
{
    gen_.generate_applicable_ops(s, ops);

    ++statistics_.aops_computations;
    statistics_.computed_operators += ops.size();
}

} // namespace probfd
