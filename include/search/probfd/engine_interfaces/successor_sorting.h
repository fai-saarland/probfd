#ifndef MDPS_ENGINE_INTERFACES_SUCCESSOR_SORTING_H
#define MDPS_ENGINE_INTERFACES_SUCCESSOR_SORTING_H

#include "probfd/distribution.h"
#include "probfd/types.h"

#include <vector>

namespace probfd {
namespace engine_interfaces {

/**
 * @brief Function object used to sort the possible successors of a transition
 * in the MDP model.
 *
 * @tparam Action - The action type of the underlying MDP model.
 *
 * @note The successor sorting operates on state IDs, so the underlying state
 * type of the MDP model is not required.
 */
template <typename Action>
struct SuccessorSorting {
    void operator()(
        const StateID& state,
        const std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors);
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __SUCCESSOR_SORTING_H__