#include "probfd/bisimulation/engine_interfaces.h"

namespace probfd {
namespace engine_interfaces {

StateID StateIDMap<bisimulation::QuotientState>::get_state_id(
    const bisimulation::QuotientState& s) const
{
    return s;
}

bisimulation::QuotientState
StateIDMap<bisimulation::QuotientState>::get_state(StateID s) const
{
    return bisimulation::QuotientState(s);
}

ActionID ActionIDMap<bisimulation::QuotientAction>::get_action_id(
    StateID,
    const bisimulation::QuotientAction& action) const
{
    return action.idx;
}

bisimulation::QuotientAction
ActionIDMap<bisimulation::QuotientAction>::get_action(
    StateID,
    const ActionID& action) const
{
    return bisimulation::QuotientAction(action);
}

TransitionGenerator<bisimulation::QuotientAction>::TransitionGenerator(
    bisimulation::BisimilarStateSpace* bisim)
    : bisim_(bisim)
{
}

void TransitionGenerator<bisimulation::QuotientAction>::
    generate_applicable_actions(
        StateID s,
        std::vector<bisimulation::QuotientAction>& res) const
{
    bisim_->get_applicable_actions(s, res);
}

void TransitionGenerator<bisimulation::QuotientAction>::
    generate_action_transitions(
        StateID s,
        const bisimulation::QuotientAction& a,
        Distribution<StateID>& res) const
{
    bisim_->get_successors(s, a, res);
}

void TransitionGenerator<bisimulation::QuotientAction>::
    generate_all_transitions(
        StateID state,
        std::vector<bisimulation::QuotientAction>& aops,
        std::vector<Distribution<StateID>>& result) const
{
    bisim_->get_applicable_actions(state, aops);
    result.resize(aops.size());
    for (int i = aops.size() - 1; i >= 0; --i) {
        bisim_->get_successors(state, aops[i], result[i]);
    }
}

} // namespace engine_interfaces

namespace bisimulation {

DefaultQuotientStateEvaluator::DefaultQuotientStateEvaluator(
    bisimulation::BisimilarStateSpace* bisim,
    Interval bound,
    value_t def)
    : bisim_(bisim)
    , bound_(bound)
    , default_(def)
{
}

EvaluationResult DefaultQuotientStateEvaluator::evaluate(
    const bisimulation::QuotientState& s) const
{
    if (bisim_->is_dead_end(s)) {
        return EvaluationResult(true, bound_.upper);
    }
    if (bisim_->is_goal_state(s)) {
        return EvaluationResult(false, bound_.lower);
    }
    return EvaluationResult(false, default_);
}

DefaultQuotientCostFunction::DefaultQuotientCostFunction(
    bisimulation::BisimilarStateSpace* bisim,
    Interval bound,
    value_t def)
    : bisim_(bisim)
    , bound_(bound)
    , default_(def)
{
}

TerminationInfo DefaultQuotientCostFunction::get_termination_info(
    const bisimulation::QuotientState& s)
{
    if (bisim_->is_dead_end(s)) {
        return TerminationInfo(false, bound_.upper);
    }
    if (bisim_->is_goal_state(s)) {
        return TerminationInfo(true, bound_.lower);
    }
    return TerminationInfo(false, default_);
}

value_t DefaultQuotientCostFunction::get_action_cost(
    StateID,
    bisimulation::QuotientAction)
{
    return 0;
}

} // namespace bisimulation

} // namespace probfd