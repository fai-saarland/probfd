#ifndef PROBFD_TRANSITION_TAIL_H
#define PROBFD_TRANSITION_TAIL_H

#include "probfd/distribution.h"
#include "probfd/state_id.h"

namespace probfd {

/**
 * @brief Contains the action and successor distribution of a transition.
 *
 * @tparam Action The action type.
 */
template <typename Action>
struct TransitionTail {
    /// The action of the transition.
    Action action;

    /// The successor distribution of the transition.
    Distribution<StateID> successor_dist;
};

} // namespace probfd

#endif