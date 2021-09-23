#ifndef MDPS_SUCCESSOR_SORT_H
#define MDPS_SUCCESSOR_SORT_H

#include "engine_interfaces/successor_sorting.h"
#include "heuristic_search_interfaceable.h"
#include "probabilistic_operator.h"

class GlobalState;

namespace probabilistic {

template <>
struct SuccessorSorting<const ProbabilisticOperator*>
    : public HeuristicSearchInterfaceable {
public:
    void operator()(
        const StateID& state,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        std::vector<Distribution<StateID>>& successors);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        std::vector<Distribution<StateID>>& successors) = 0;
};

using ProbabilisticOperatorSuccessorSorting =
    SuccessorSorting<const ProbabilisticOperator*>;

} // namespace probabilistic

#endif // __SUCCESSOR_SORT_H__