#include "engine_interfaces.h"

namespace probabilistic {

StateID StateIDMap<bisimulation::QuotientState>::get_state_id(
    const bisimulation::QuotientState& s) const
{
    return s.hash();
}

bisimulation::QuotientState
StateIDMap<bisimulation::QuotientState>::get_state(const StateID& s) const
{
    return bisimulation::QuotientState(s);
}

ActionID ActionIDMap<bisimulation::QuotientAction>::get_action_id(
    const StateID&,
    const bisimulation::QuotientAction& action) const
{
    return action.idx;
}

bisimulation::QuotientAction
ActionIDMap<bisimulation::QuotientAction>::get_action(
    const StateID&,
    const ActionID& action) const
{
    return bisimulation::QuotientAction(action);
}

TransitionGenerator<bisimulation::QuotientAction>::TransitionGenerator(
    bisimulation::BisimilarStateSpace* bisim)
    : bisim_(bisim)
{
}

void TransitionGenerator<bisimulation::QuotientAction>::operator()(
    const StateID& s,
    std::vector<bisimulation::QuotientAction>& res) const
{
    bisim_->get_applicable_actions(s, res);
}

void TransitionGenerator<bisimulation::QuotientAction>::operator()(
    const StateID& s,
    const bisimulation::QuotientAction& a,
    Distribution<StateID>& res) const
{
    bisim_->get_successors(s, a, res);
}

void TransitionGenerator<bisimulation::QuotientAction>::operator()(
    const StateID& state,
    std::vector<bisimulation::QuotientAction>& aops,
    std::vector<Distribution<StateID>>& result) const
{
    bisim_->get_applicable_actions(state, aops);
    result.resize(aops.size());
    for (int i = aops.size() - 1; i >= 0; --i) {
        bisim_->get_successors(state, aops[i], result[i]);
    }
}

} // namespace probabilistic