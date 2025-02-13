#ifndef PROBFD_TRANSITION_TAIL_H
#define PROBFD_TRANSITION_TAIL_H

#include "probfd/distribution.h"
#include "probfd/state_id.h"

#include <compare>

namespace probfd {

struct SuccessorDistribution {
    /// The successor distribution over the non-source states of the transition.
    Distribution<StateID> non_source_successor_dist;

    /// Probability of the transition source state.
    value_t non_source_probability = 0_vt;

    void add_non_source_probability(StateID state_id, value_t probability)
    {
        non_source_successor_dist.add_probability(state_id, probability);
        non_source_probability += probability;
    }

    void clear()
    {
        non_source_successor_dist.clear();
        non_source_probability = 0_vt;
    }

    bool is_dirac() const
    {
        return non_source_probability == 0_vt ||
               non_source_successor_dist.empty();
    }

    friend bool
    operator==(const SuccessorDistribution&, const SuccessorDistribution&) =
        default;

    friend auto
    operator<=>(const SuccessorDistribution&, const SuccessorDistribution&) =
        default;
};

/**
 * @brief Contains the action and successor distribution of a transition.
 *
 * @tparam Action The action type.
 */
template <typename Action>
struct TransitionTail {
    /// The action of the transition.
    Action action;

    /// The successor probability distribution.
    SuccessorDistribution successor_dist;
};

} // namespace probfd

#endif