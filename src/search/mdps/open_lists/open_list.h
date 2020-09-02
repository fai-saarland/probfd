#pragma once

#include "../../global_state.h"
#include "../algorithms/types_heuristic_search.h"
#include "../probabilistic_operator.h"
#include "../value_type.h"

namespace probabilistic {
namespace algorithms {
template<>
class OpenList<GlobalState, const ProbabilisticOperator*> {
public:
    virtual ~OpenList() = default;
    bool empty() const;
    virtual unsigned size() const = 0;
    virtual void clear() = 0;
    virtual StateID pop() = 0;
    virtual void push(const StateID& state_id, const GlobalState& state) = 0;
    virtual void push(
        const StateID& state_id,
        const GlobalState& parent,
        const ProbabilisticOperator* op,
        const value_type::value_t& prob,
        const GlobalState& state);
};
} // namespace algorithms

namespace open_lists {

using GlobalStateOpenList =
    algorithms::OpenList<GlobalState, const ProbabilisticOperator*>;

} // namespace open_lists
} // namespace probabilistic
