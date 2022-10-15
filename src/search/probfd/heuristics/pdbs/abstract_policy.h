#ifndef MDPS_HEURISTICS_PDBS_ABSTRACT_POLICY_H
#define MDPS_HEURISTICS_PDBS_ABSTRACT_POLICY_H

#include "probfd/heuristics/pdbs/abstract_state.h"

#include "probfd/value_type.h"

#include "utils/iterators.h"

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace probfd {
namespace heuristics {

namespace pdbs {

struct AbstractOperator;

class AbstractPolicy {
public:
    using OperatorList = std::vector<const AbstractOperator*>;

private:
    std::unordered_map<AbstractState, OperatorList> optimal_operators;

public:
    using iterator = decltype(optimal_operators)::iterator;
    using const_iterator = decltype(optimal_operators)::const_iterator;

    using state_iterator = utils::key_iterator<iterator>;
    using const_state_iterator = utils::key_iterator<const_iterator>;

    using operators_iterator = utils::val_iterator<iterator>;
    using const_operators_iterator = utils::val_iterator<const_iterator>;

    const_iterator find(const AbstractState& state) const;

    OperatorList& operator[](const AbstractState& state);
    const OperatorList& operator[](const AbstractState& state) const;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    state_iterator state_begin();
    state_iterator state_end();

    const_state_iterator state_begin() const;
    const_state_iterator state_end() const;

    operators_iterator operator_begin();
    operators_iterator operator_end();

    const_operators_iterator operator_begin() const;
    const_operators_iterator operator_end() const;

    // AbstractTrace sample_trace(const AbstractState& state);

    // AbstractState sample_successor(const AbstractState& state);
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif