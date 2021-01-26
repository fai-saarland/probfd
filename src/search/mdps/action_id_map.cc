#include "action_id_map.h"

#include "globals.h"

namespace probabilistic {

ActionIDMap<const ProbabilisticOperator*>::ActionIDMap()
    : ActionIDMap(g_operators)
{
}

ActionIDMap<const ProbabilisticOperator*>::ActionIDMap(
    const std::vector<ProbabilisticOperator>& ops)
    : first_(!ops.empty() ? &ops[0] : nullptr)
{
#ifndef NDEBUG
    num_operators_ = ops.size();
#endif
}

ActionID
ActionIDMap<const ProbabilisticOperator*>::get_action_id(
    const StateID&,
    const ProbabilisticOperator* const& op)
{
    assert(int(op - first_) >= 0 && int(op - first_) < int(num_operators_));
    return ActionID(op - first_);
}

const ProbabilisticOperator*
ActionIDMap<const ProbabilisticOperator*>::get_action(
    const StateID&,
    const ActionID& action_id)
{
    assert(action_id < num_operators_);
    return first_ + action_id;
}

} // namespace probabilistic
