#ifndef PROBFD_ALGORITHMS_HEURISTIC_SEARCH_STATE_INFORMATION_H
#define PROBFD_ALGORITHMS_HEURISTIC_SEARCH_STATE_INFORMATION_H

#include "probfd/algorithms/state_properties.h"
#include "probfd/algorithms/types.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/labelled_successor_distribution.h"
#include "probfd/state_id.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <optional>

namespace probfd::algorithms::heuristic_search {

template <typename, bool StorePolicy = false>
struct StatesPolicy {};

template <typename Action>
struct StatesPolicy<Action, true> {
    std::optional<Action> policy = std::nullopt;

    std::optional<Action> get_policy() const
    {
        return policy;
    }

    bool update_policy(const std::optional<Action>& a)
    {
        const bool changed = policy != a;
        policy = a;
        return changed;
    }

    bool update_policy(
        const std::optional<LabelledSuccessorDistribution<Action>>& transition)
    {
        return update_policy(
            transition.transform([](auto& t) { return t.action; }));
    }

    bool update_policy(std::nullopt_t)
    {
        return update_policy(std::optional<Action>{});
    }
};

struct StateFlags {
    static constexpr unsigned int TERMINAL = 1;
    static constexpr unsigned int FRINGE = 2;

    static constexpr unsigned int MASK = 0b11;
    static constexpr unsigned int BITS = 2;

    uint8_t info = 0;

    [[nodiscard]]
    bool is_termination_optimal() const
    {
        return (info & MASK) == TERMINAL;
    }

    [[nodiscard]]
    bool is_on_fringe() const
    {
        return (info & MASK) == FRINGE;
    }

    void set_on_fringe()
    {
        info = (info & ~MASK) | FRINGE;
    }

    void set_termination_optimal()
    {
        assert(!is_termination_optimal());
        info = (info & ~MASK) | TERMINAL;
    }

    void removed_from_fringe()
    {
        assert(!is_termination_optimal());
        info = (info & ~MASK);
    }
};

template <typename Action, bool StorePolicy_, bool UseInterval_>
struct PerStateBaseInformation
    : StatesPolicy<Action, StorePolicy_>
    , StateFlags {
    static constexpr bool StorePolicy = StorePolicy_;
    static constexpr bool UseInterval = UseInterval_;

    AlgorithmValue<UseInterval> value;

    PerStateBaseInformation()
        requires std::same_as<AlgorithmValue<UseInterval>, value_t>
        : value(std::numeric_limits<value_t>::quiet_NaN())
    {
    }

    PerStateBaseInformation()
        requires std::same_as<AlgorithmValue<UseInterval>, Interval>
        : value(
              std::numeric_limits<value_t>::quiet_NaN(),
              std::numeric_limits<value_t>::quiet_NaN())
    {
    }

    /// Checks if the value bounds are epsilon-close.
    [[nodiscard]]
    bool bounds_approximately_equal(value_t epsilon) const
        requires UseInterval
    {
        return value.bounds_approximately_equal(epsilon);
    }

    [[nodiscard]]
    value_t get_value() const
    {
        if constexpr (UseInterval) {
            return value.lower;
        } else {
            return value;
        }
    }

    [[nodiscard]]
    Interval get_bounds() const
    {
        if constexpr (UseInterval) {
            return value;
        } else {
            return Interval(value, INFINITE_VALUE);
        }
    }

    [[nodiscard]]
    bool is_value_initialized() const
    {
        if constexpr (std::same_as<AlgorithmValue<UseInterval>, value_t>) {
            return !std::isnan(value);
        } else {
            return !std::isnan(value.lower);
        }
    }
};

} // namespace probfd::algorithms::heuristic_search

#endif // PROBFD_ALGORITHMS_HEURISTIC_SEARCH_STATE_INFORMATION_H