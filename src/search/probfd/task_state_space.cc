#include "probfd/task_state_space.h"
#include "probfd/task_proxy.h"

#include "downward/algorithms/int_packer.h"

#include "downward/evaluator.h"

#include "downward/tasks/root_task.h"

#include <algorithm>
#include <cstring>
#include <map>

namespace probfd {

InducedTaskStateSpace::InducedTaskStateSpace(
    std::shared_ptr<ProbabilisticTask> task,
    const std::vector<std::shared_ptr<Evaluator>>& path_dependent_evaluators)
    : task_proxy(*task)
    , gen_(task_proxy)
    , state_registry_(task_proxy)
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
    statistics_.generated_states += result.size();
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
        auto num = compute_successor_dist(state, op, successors.emplace_back());
        statistics_.generated_states += num;
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += aops.size();
}

const State& InducedTaskStateSpace::get_initial_state()
{
    return state_registry_.get_initial_state();
}

size_t InducedTaskStateSpace::get_num_registered_states() const
{
    return state_registry_.size();
}

void InducedTaskStateSpace::print_statistics(std::ostream& out) const
{
    statistics_.print(out);
}

void InducedTaskStateSpace::Statistics::print(std::ostream& out) const
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

void InducedTaskStateSpace::compute_applicable_operators(
    const State& s,
    std::vector<OperatorID>& ops)
{
    gen_.generate_applicable_ops(s, ops);

    ++statistics_.aops_computations;
    statistics_.computed_operators += ops.size();
}

void InducedTaskStateSpace::compute_successor_states(
    const State& state,
    OperatorID op_id,
    std::vector<StateID>& succs)
{
    const ProbabilisticOperatorProxy op = task_proxy.get_operators()[op_id];
    const auto outcomes = op.get_outcomes();
    const size_t num_outcomes = outcomes.size();
    succs.reserve(num_outcomes);

    for (const ProbabilisticOutcomeProxy outcome : outcomes) {
        State succ = state_registry_.get_successor_state(state, outcome);

        for (const auto& h : notify_) {
            OperatorID det_op_id(outcome.get_determinization_id());
            h->notify_state_transition(state, det_op_id, succ);
        }

        succs.emplace_back(succ.get_id());
    }

    ++statistics_.transition_computations;
    statistics_.computed_successors += num_outcomes;
}

size_t InducedTaskStateSpace::compute_successor_dist(
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

    return num_outcomes;
}

} // namespace probfd

#undef DEBUG_CACHE_CONSISTENCY_CHECK
