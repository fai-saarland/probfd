#include "abstract_policy.h"

namespace probabilistic {
namespace heuristics {
namespace pdbs {

AbstractPolicy::const_iterator
AbstractPolicy::find(const AbstractState& state) const
{
    return optimal_operators.find(state);
}

AbstractPolicy::OperatorList&
AbstractPolicy::operator[](const AbstractState& state)
{
    return optimal_operators[state];
}

const AbstractPolicy::OperatorList&
AbstractPolicy::operator[](const AbstractState& state) const
{
    return optimal_operators.find(state)->second;
}

AbstractPolicy::iterator AbstractPolicy::begin()
{
    return optimal_operators.begin();
}

AbstractPolicy::iterator AbstractPolicy::end()
{
    return optimal_operators.end();
}

AbstractPolicy::const_iterator AbstractPolicy::begin() const
{
    return optimal_operators.begin();
}

AbstractPolicy::const_iterator AbstractPolicy::end() const
{
    return optimal_operators.end();
}

AbstractPolicy::state_iterator AbstractPolicy::state_begin()
{
    return state_iterator(optimal_operators.begin());
}

AbstractPolicy::state_iterator AbstractPolicy::state_end()
{
    return state_iterator(optimal_operators.end());
}

AbstractPolicy::const_state_iterator AbstractPolicy::state_begin() const
{
    return const_state_iterator(optimal_operators.begin());
}

AbstractPolicy::const_state_iterator AbstractPolicy::state_end() const
{
    return const_state_iterator(optimal_operators.end());
}

AbstractPolicy::operators_iterator AbstractPolicy::operator_begin()
{
    return operators_iterator(optimal_operators.begin());
}

AbstractPolicy::operators_iterator AbstractPolicy::operator_end()
{
    return operators_iterator(optimal_operators.end());
}

AbstractPolicy::const_operators_iterator AbstractPolicy::operator_begin() const
{
    return const_operators_iterator(optimal_operators.begin());
}

AbstractPolicy::const_operators_iterator AbstractPolicy::operator_end() const
{
    return const_operators_iterator(optimal_operators.end());
}

} // namespace pdbs
} // namespace heuristics
} // namespace probabilistic