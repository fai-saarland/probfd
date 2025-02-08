#include "probfd/task_state_space.h"

#include "probfd/distribution.h"
#include "probfd/task_proxy.h"
#include "probfd/transition_tail.h"
#include "probfd/type_traits.h"

#include "downward/evaluator.h"
#include "downward/operator_id.h"
#include "downward/state_id.h"

#include <iostream>

namespace probfd {

void TaskStateSpace::Statistics::print(std::ostream& out) const
{
    out << "  Applicable operators: " << generated_operators << " generated, "
        << computed_operators << " computed, " << aops_generator_calls
        << " generator calls." << std::endl;
    out << "  Generated " << generated_states
        << " successor state(s): " << computed_successors << " computed, "
        << single_transition_generator_calls << " single-transition calls, "
        << all_transitions_generator_calls << " all-transitions calls."
        << std::endl;
}

TaskStateSpace::TaskStateSpace(
    std::shared_ptr<ProbabilisticTask> task,
    std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators)
    : task_proxy_(*task)
    , state_registry_(task_proxy_)
    , gen_(task_proxy_)
    , notify_(std::move(path_dependent_evaluators))
{
}

StateID TaskStateSpace::get_state_id(const State& state)
{
    return state.get_id();
}

State TaskStateSpace::get_state(StateID state_id)
{
    return state_registry_.lookup_state(::StateID(state_id));
}

void TaskStateSpace::generate_applicable_actions(
    const State& state,
    std::vector<OperatorID>& result)
{
    compute_applicable_operators(state, result);

    ++statistics_.aops_generator_calls;
    statistics_.generated_operators += result.size();
}

void TaskStateSpace::generate_action_transitions(
    const State& state,
    OperatorID op_id,
    Distribution<StateID>& result)
{
    compute_successor_dist(state, op_id, result);
    ++statistics_.single_transition_generator_calls;
}

void TaskStateSpace::generate_all_transitions(
    const State& state,
    std::vector<OperatorID>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    compute_applicable_operators(state, aops);
    successors.reserve(aops.size());

    for (const auto& op : aops) {
        compute_successor_dist(state, op, successors.emplace_back());
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += aops.size();
}

void TaskStateSpace::generate_all_transitions(
    const State& state,
    std::vector<TransitionTailType>& transitions)
{
    gen_.generate_transitions(state, transitions, *this);

    ++statistics_.aops_computations;
    ++statistics_.all_transitions_generator_calls;
    statistics_.computed_operators += transitions.size();
    statistics_.generated_operators += transitions.size();
}

const State& TaskStateSpace::get_initial_state()
{
    return state_registry_.get_initial_state();
}

size_t TaskStateSpace::get_num_registered_states() const
{
    return state_registry_.size();
}

void TaskStateSpace::print_statistics(std::ostream& out) const
{
    out << "  Registered state(s): " << get_num_registered_states()
        << std::endl;
    statistics_.print(out);
}

void TaskStateSpace::compute_successor_dist(
    const State& state,
    OperatorID op_id,
    Distribution<StateID>& successor_dist)
{
    const ProbabilisticOperatorProxy op = task_proxy_.get_operators()[op_id];
    const auto outcomes = op.get_outcomes();
    const size_t num_outcomes = outcomes.size();
    successor_dist.reserve(num_outcomes);

    for (const ProbabilisticOutcomeProxy outcome : outcomes) {
        value_t probability = outcome.get_probability();
        State succ =
            state_registry_.get_successor_state(state, outcome.get_effects());

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

void TaskStateSpace::compute_applicable_operators(
    const State& s,
    std::vector<OperatorID>& ops)
{
    gen_.generate_applicable_ops(s, ops);

    ++statistics_.aops_computations;
    statistics_.computed_operators += ops.size();
}

} // namespace probfd
