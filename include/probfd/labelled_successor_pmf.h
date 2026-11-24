#ifndef PROBFD_LABELLED_SUCCESSOR_PMF_H
#define PROBFD_LABELLED_SUCCESSOR_PMF_H

#include "probfd/pmf.h"
#include "probfd/state_id.h"

#include <compare>

namespace probfd {

struct SuccessorPMF {
    /// The successor PMF over the non-source states of the transition.
    PMF<StateID> non_source_successor_pmf;

    /// Probability of the transition source state.
    value_t non_source_probability = 0_vt;

    void add_non_source_probability(StateID state_id, value_t probability)
    {
        non_source_successor_pmf.add_probability(state_id, probability);
        non_source_probability += probability;
    }

    void clear()
    {
        non_source_successor_pmf.clear();
        non_source_probability = 0_vt;
    }

    bool is_dirac() const
    {
        return non_source_probability == 0_vt ||
               non_source_successor_pmf.empty();
    }

    auto sample(downward::utils::RandomNumberGenerator& rng) const
    {
        assert(!non_source_successor_pmf.empty());

        const value_t r = non_source_probability * rng.random();

        auto it = non_source_successor_pmf.begin();
        value_t sum = it->probability;

        while (sum <= r) {
            sum += (++it)->probability;
        }

        return it;
    }

    friend bool operator==(const SuccessorPMF&, const SuccessorPMF&) = default;

    friend auto operator<=>(const SuccessorPMF&, const SuccessorPMF&) = default;
};

/**
 * @brief Contains the action and successor PMF of a transition.
 *
 * @tparam Action The action type.
 */
template <typename Action>
struct LabelledSuccessorPMF {
    /// The action of the transition.
    Action action;

    /// The successor probability distribution.
    SuccessorPMF successor_pmf;
};

} // namespace probfd

#endif