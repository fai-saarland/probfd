#pragma once

#include "combination_strategy.h"

#include <numeric>
#include <vector>

class GlobalState;

namespace probabilistic {
namespace pdbs {

template <typename Reducer>
class ReductionStrategy : public CombinationStrategy {
    const value_type::value_t neutral_element;
    Reducer reducer;

public:
    ReductionStrategy(
        value_type::value_t neutral_element,
        Reducer reducer = Reducer())
        : neutral_element(neutral_element)
        , reducer(reducer)
    {
    }

    virtual ~ReductionStrategy() = default;

    virtual value_type::value_t combine(
        const std::vector<value_type::value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const override
    {
        auto transformer = [&pdb_estimates](int i) { return pdb_estimates[i]; };

        return std::transform_reduce(
            subcollection.begin(),
            subcollection.end(),
            neutral_element,
            reducer,
            transformer);
    }

    virtual value_type::value_t
    combine(value_type::value_t left, value_type::value_t right) const override
    {
        return reducer(left, right);
    }
};

} // namespace pdbs
} // namespace probabilistic
