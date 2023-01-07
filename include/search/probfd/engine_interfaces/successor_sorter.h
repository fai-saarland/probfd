#ifndef PROBFD_ENGINE_INTERFACES_SUCCESSOR_SORTER_H
#define PROBFD_ENGINE_INTERFACES_SUCCESSOR_SORTER_H

#include "probfd/distribution.h"
#include "probfd/types.h"

#include <vector>

namespace probfd {
namespace engine_interfaces {

class HeuristicSearchInterface;

/**
 * @brief Function object used to sort the possible successors of a transition
 * in the MDP model.
 *
 * @tparam Action - The action type of the underlying MDP model.
 *
 * @note The successor sorter operates on state IDs, so the underlying state
 * type of the MDP model is not required.
 */
template <typename Action>
class SuccessorSorter {
public:
    virtual ~SuccessorSorter() = default;

    virtual void sort(
        const StateID& state,
        const std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors,
        HeuristicSearchInterface& hs_interface) = 0;
};

} // namespace engine_interfaces
} // namespace probfd

class OperatorID;

namespace probfd {

using TaskSuccessorSorter = engine_interfaces::SuccessorSorter<OperatorID>;

} // namespace probfd

#endif // __SUCCESSOR_SORTING_H__