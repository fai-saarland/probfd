#ifndef PROBFD_ENGINE_INTERFACES_SUCCESSOR_SAMPLER_H
#define PROBFD_ENGINE_INTERFACES_SUCCESSOR_SAMPLER_H

#include "probfd/distribution.h"
#include "probfd/type_traits.h"
#include "probfd/types.h"

namespace probfd {
namespace engine_interfaces {

class HeuristicSearchInterface;

/**
 * @brief An interface used to sample a state from a successor distribution.
 *
 * Used by sample-based heuristic search algorithms like LRTDP.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
class SuccessorSampler {
public:
    virtual ~SuccessorSampler() = default;

    /**
     * @brief Samples a successor from the successor distribution of a
     * transition.
     *
     * @param state - The source state of the transition.
     * @param action - The action of the transition.
     * @param successor_dist - The successor distribution of the transition.
     * @param hs_interface - The interface to the heuristic search algorithm.
     * Can be used to query additional information about the involved states and
     * actions.
     */
    virtual StateID sample(
        StateID state,
        param_type<Action> action,
        const Distribution<StateID>& successor_dist,
        HeuristicSearchInterface& hs_interface) = 0;

    /**
     * @brief Prints statistics, e.g. the number of queries made to the
     * interface.
     */
    virtual void print_statistics(std::ostream&) const {}
};

} // namespace engine_interfaces
} // namespace probfd

class OperatorID;

namespace probfd {
// Type alias for successor samplers for probabilistic planning tasks.
using TaskSuccessorSampler = engine_interfaces::SuccessorSampler<OperatorID>;
} // namespace probfd

#endif // __SUCCESSOR_SAMPLER_H__