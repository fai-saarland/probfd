#ifndef PROBFD_ALGORITHMS_SUCCESSOR_SAMPLER_H
#define PROBFD_ALGORITHMS_SUCCESSOR_SAMPLER_H

#include "probfd/types.h"

namespace probfd {
    
template <typename>
class Distribution;

namespace algorithms {

class StateProperties;

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
     * @param properties - The interface to the heuristic search algorithm.
     * Can be used to query additional information about the involved states and
     * actions.
     */
    virtual StateID sample(
        StateID state,
        Action action,
        const Distribution<StateID>& successor_dist,
        StateProperties& properties) = 0;

    /**
     * @brief Prints statistics, e.g. the number of queries made to the
     * interface.
     */
    virtual void print_statistics(std::ostream&) const {}
};

} // namespace algorithms
} // namespace probfd

#endif // __SUCCESSOR_SAMPLER_H__