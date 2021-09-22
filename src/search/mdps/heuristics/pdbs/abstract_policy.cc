#include "abstract_policy.h"

namespace probabilistic {
namespace pdbs {

AbstractTrace::AbstractTrace(AbstractState start)
    : start(start)
    , total_probability(value_type::zero)
{
}

unsigned int AbstractTrace::length() const
{
    return trace.size();
}

AbstractState AbstractTrace::get_start() const
{
    return start;
}

Transition& AbstractTrace::get_transition(int i)
{
    return trace[i];
}

const Transition& AbstractTrace::get_transition(int i) const
{
    return trace[i];
}

void AbstractTrace::push_back(const Transition& transition)
{
    trace.push_back(transition);
    total_probability += transition.probability;
}

Transition& AbstractTrace::emplace_back(
    const AbstractOperator* label,
    value_type::value_t probability,
    AbstractState target)
{
    total_probability += probability;
    return trace.emplace_back(label, probability, target);
}

value_type::value_t AbstractTrace::get_total_probability() const
{
    return total_probability;
}

AbstractTrace::transition_iterator AbstractTrace::begin()
{
    return trace.begin();
}

AbstractTrace::transition_iterator AbstractTrace::end()
{
    return trace.end();
}

AbstractTrace::transition_const_iterator AbstractTrace::cbegin() const
{
    return trace.cbegin();
}

AbstractTrace::transition_const_iterator AbstractTrace::cend() const
{
    return trace.cend();
}

AbstractPolicy::AbstractPolicy(
    std::unordered_map<AbstractState, const AbstractOperator*> policy)
    : policy(std::move(policy))
{
}

AbstractPolicy::AbstractPolicy(
    const std::map<AbstractState, const AbstractOperator*>& policy)
    : policy(policy.begin(), policy.end())
{
}

AbstractPolicy::AbstractPolicy(
    const std::vector<std::pair<AbstractState, const AbstractOperator*>>&
        policy)
    : policy(policy.begin(), policy.end())
{
}

const AbstractOperator*
AbstractPolicy::get_operator_if_present(const AbstractState& state) const
{
    auto it = policy.find(state);

    if (it != policy.end()) {
        return it->second;
    }

    return nullptr;
}

const AbstractOperator*& AbstractPolicy::operator[](const AbstractState& state)
{
    return policy[state];
}

const AbstractOperator* const&
AbstractPolicy::operator[](const AbstractState& state) const
{
    return policy.find(state)->second;
}

AbstractPolicy::iterator AbstractPolicy::begin()
{
    return policy.begin();
}

AbstractPolicy::iterator AbstractPolicy::end()
{
    return policy.end();
}

AbstractPolicy::const_iterator AbstractPolicy::begin() const
{
    return policy.begin();
}

AbstractPolicy::const_iterator AbstractPolicy::end() const
{
    return policy.end();
}

AbstractPolicy::state_iterator AbstractPolicy::state_begin()
{
    return state_iterator(policy.begin());
}

AbstractPolicy::state_iterator AbstractPolicy::state_end()
{
    return state_iterator(policy.end());
}

AbstractPolicy::const_state_iterator AbstractPolicy::state_begin() const
{
    return const_state_iterator(policy.begin());
}

AbstractPolicy::const_state_iterator AbstractPolicy::state_end() const
{
    return const_state_iterator(policy.end());
}

AbstractPolicy::operator_iterator AbstractPolicy::operator_begin()
{
    return operator_iterator(policy.begin());
}

AbstractPolicy::operator_iterator AbstractPolicy::operator_end()
{
    return operator_iterator(policy.end());
}

AbstractPolicy::const_operator_iterator AbstractPolicy::operator_begin() const
{
    return const_operator_iterator(policy.begin());
}

AbstractPolicy::const_operator_iterator AbstractPolicy::operator_end() const
{
    return const_operator_iterator(policy.end());
}

} // namespace pdbs
} // namespace probabilistic