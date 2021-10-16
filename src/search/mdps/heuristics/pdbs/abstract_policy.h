#ifndef MDPS_HEURISTICS_PDBS_ABSTRACT_POLICY_H
#define MDPS_HEURISTICS_PDBS_ABSTRACT_POLICY_H

#include "../../../utils/iterators.h"

#include "../../value_type.h"
#include "abstract_state.h"

#include <map>
#include <unordered_map>
#include <vector>

namespace probabilistic {
namespace pdbs {

class AbstractOperator;

struct Transition {
    const AbstractOperator* label;
    value_type::value_t probability;
    AbstractState target;

    Transition(
        const AbstractOperator* label,
        value_type::value_t probability,
        AbstractState target)
        : label(label)
        , probability(probability)
        , target(target)
    {
    }
};

class AbstractTrace {
    const AbstractState start;
    std::vector<Transition> trace;
    value_type::value_t total_probability;

public:
    using transition_iterator = decltype(trace)::iterator;
    using transition_const_iterator = decltype(trace)::const_iterator;

    explicit AbstractTrace(AbstractState start);

    unsigned int length() const;

    AbstractState get_start() const;

    Transition& get_transition(int i);
    const Transition& get_transition(int i) const;

    void push_back(const Transition& transition);

    Transition& emplace_back(
        const AbstractOperator* label,
        value_type::value_t probability,
        AbstractState target);

    value_type::value_t get_total_probability() const;

    transition_iterator begin();
    transition_iterator end();

    transition_const_iterator cbegin() const;
    transition_const_iterator cend() const;
};

class AbstractPolicy {
    std::unordered_map<AbstractState, const AbstractOperator*> policy;

public:
    using iterator = decltype(policy)::iterator;
    using const_iterator = decltype(policy)::const_iterator;

    using state_iterator = utils::key_iterator<iterator>;
    using const_state_iterator = utils::key_iterator<const_iterator>;

    using operator_iterator = utils::val_iterator<iterator>;
    using const_operator_iterator = utils::val_iterator<const_iterator>;

    AbstractPolicy() = default;

    AbstractPolicy(
        std::unordered_map<AbstractState, const AbstractOperator*> policy);
    AbstractPolicy(
        const std::map<AbstractState, const AbstractOperator*>& policy);
    AbstractPolicy(
        const std::vector<std::pair<AbstractState, const AbstractOperator*>>&
            policy);

    template <typename Iterator>
    AbstractPolicy(Iterator begin, Iterator end)
        : policy(begin, end)
    {
    }

    const AbstractOperator*
    get_operator_if_present(const AbstractState& state) const;

    const AbstractOperator*& operator[](const AbstractState& state);
    const AbstractOperator* const& operator[](const AbstractState& state) const;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    state_iterator state_begin();
    state_iterator state_end();

    const_state_iterator state_begin() const;
    const_state_iterator state_end() const;

    operator_iterator operator_begin();
    operator_iterator operator_end();

    const_operator_iterator operator_begin() const;
    const_operator_iterator operator_end() const;

    // AbstractTrace sample_trace(const AbstractState& state);

    // AbstractState sample_successor(const AbstractState& state);
};

} // namespace pdbs
} // namespace probabilistic

#endif