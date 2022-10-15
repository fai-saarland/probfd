#ifndef MDPS_SUCCESSOR_SORT_H
#define MDPS_SUCCESSOR_SORT_H

#include "probfd/engine_interfaces/successor_sorting.h"

#include "probfd/heuristic_search_interfaceable.h"
#include "probfd/probabilistic_operator.h"

class GlobalState;

namespace probfd {
namespace engine_interfaces {

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

} // namespace engine_interfaces

using ProbabilisticOperatorSuccessorSorting =
    engine_interfaces::SuccessorSorting<const ProbabilisticOperator*>;

} // namespace probfd

#endif // __SUCCESSOR_SORT_H__