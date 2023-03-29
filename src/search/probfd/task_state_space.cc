#include "probfd/task_state_space.h"
#include "probfd/task_proxy.h"

#include "algorithms/int_packer.h"

#include "evaluator.h"

#include "tasks/root_task.h"

#include <algorithm>
#include <cstring>
#include <map>

namespace probfd {

TaskStateSpace::TaskStateSpace(
    std::shared_ptr<ProbabilisticTask> task,
    const std::vector<std::shared_ptr<Evaluator>>& path_dependent_evaluators)
    : task_proxy(*task)
    , gen_(task_proxy)
    , state_registry_(task_proxy)
    , notify_(path_dependent_evaluators)
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

ActionID TaskStateSpace::get_action_id(StateID, OperatorID op_id)
{
    return ActionID(op_id.get_index());
}

OperatorID TaskStateSpace::get_action(StateID, ActionID action_id)
{
    return OperatorID(action_id);
}

void TaskStateSpace::generate_applicable_actions(
    StateID state_id,
    std::vector<OperatorID>& result)
{
    State state = state_registry_.lookup_state(::StateID(state_id));
    compute_applicable_operators(state, result);

    ++statistics_.aops_generator_calls;
    statistics_.generated_operators += result.size();
}

void TaskStateSpace::generate_action_transitions(
    StateID state_id,
    OperatorID op_id,
    Distribution<StateID>& result)
{
    State state = state_registry_.lookup_state(::StateID(state_id));
    std::vector<ItemProbabilityPair<StateID>> temp;
    compute_successor_states(state, op_id, temp);
    result = Distribution<StateID>(std::move(temp));

    ++statistics_.single_transition_generator_calls;
    statistics_.generated_states += result.size();
}

void TaskStateSpace::generate_all_transitions(
    StateID state_id,
    std::vector<OperatorID>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    State state = state_registry_.lookup_state(::StateID(state_id));
    compute_applicable_operators(state, aops);
    successors.reserve(aops.size());

    std::vector<ItemProbabilityPair<StateID>> temp;
    for (const auto& op : aops) {
        compute_successor_states(state, op, temp);
        const auto& new_dist = successors.emplace_back(std::move(temp));
        statistics_.generated_states += new_dist.size();
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += aops.size();
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
    statistics_.print(out);
}

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

void TaskStateSpace::compute_successor_states(
    const State& state,
    OperatorID op_id,
    std::vector<ItemProbabilityPair<StateID>>& succs)
{
    const ProbabilisticOperatorProxy op = task_proxy.get_operators()[op_id];
    succs.reserve(op.get_outcomes().size());

    for (const ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
        value_t probability = outcome.get_probability();
        State succ = state_registry_.get_successor_state(state, outcome);

        for (const auto& h : notify_) {
            OperatorID det_op_id(outcome.get_determinization_id());
            h->notify_state_transition(state, det_op_id, succ);
        }

        succs.emplace_back(succ.get_id(), probability);
    }

    ++statistics_.transition_computations;
    statistics_.computed_successors += succs.size();
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

#undef DEBUG_CACHE_CONSISTENCY_CHECK
