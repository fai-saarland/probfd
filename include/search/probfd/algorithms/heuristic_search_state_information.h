#ifndef PROBFD_ALGORITHMS_HEURISTIC_SEARCH_STATE_INFORMATION_H
#define PROBFD_ALGORITHMS_HEURISTIC_SEARCH_STATE_INFORMATION_H

#include "probfd/algorithms/types.h"

#include "probfd/algorithms/state_properties.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/types.h"

#include <cassert>
#include <cstdint>
#include <optional>

namespace probfd::algorithms::heuristic_search {

template <typename, bool StorePolicy = false>
struct StatesPolicy {};

template <typename Action>
struct StatesPolicy<Action, true> {
    std::optional<Action> policy = std::nullopt;

    void set_policy(const std::optional<Action>& a) { policy = a; }

    void set_policy(const std::optional<Transition<Action>>& transition)
    {
        set_policy(
            transition.template transform([](auto& t) { return t.action; }));
    }

    void set_policy(std::nullopt_t) { policy = std::nullopt; }

    void clear_policy() { set_policy(std::nullopt); }

    std::optional<Action> get_policy() const { return policy; }

    bool update_policy(const std::optional<Action>& a)
    {
        bool changed = policy != a;
        set_policy(a);
        return changed;
    }

    bool update_policy(const std::optional<Transition<Action>>& transition)
    {
        return update_policy(
            transition.template transform([](auto& t) { return t.action; }));
    }

    bool update_policy(std::nullopt_t)
    {
        return update_policy(std::optional<Action>{});
    }
};

struct StateFlags {
    static constexpr uint8_t INITIALIZED = 1;
    static constexpr uint8_t DEAD = 2;
    static constexpr uint8_t GOAL = 4;
    static constexpr uint8_t FRINGE = 5;
    static constexpr uint8_t MASK = 7;
    static constexpr uint8_t BITS = 3;

    uint8_t info = 0;

    [[nodiscard]]
    bool is_value_initialized() const
    {
        return (info & MASK) != 0;
    }

    [[nodiscard]]
    bool is_dead_end() const
    {
        return (info & MASK) == DEAD;
    }

    [[nodiscard]]
    bool is_goal_state() const
    {
        return (info & MASK) == GOAL;
    }

    [[nodiscard]]
    bool is_terminal() const
    {
        return is_dead_end() || is_goal_state();
    }

    [[nodiscard]]
    bool is_on_fringe() const
    {
        return (info & MASK) == FRINGE;
    }

    void set_goal()
    {
        assert(!is_value_initialized());
        info = (info & ~MASK) | GOAL;
    }

    void set_on_fringe()
    {
        assert(!is_value_initialized());
        info = (info & ~MASK) | FRINGE;
    }

    void set_dead_end()
    {
        assert(!is_goal_state() && !is_dead_end());
        info = (info & ~MASK) | DEAD;
    }

    void removed_from_fringe()
    {
        assert(is_value_initialized() && !is_terminal());
        info = (info & ~MASK) | INITIALIZED;
    }
};

template <typename Action, bool StorePolicy_, bool UseInterval_>
struct PerStateBaseInformation
    : public StatesPolicy<Action, StorePolicy_>
    , public StateFlags {
    static constexpr bool StorePolicy = StorePolicy_;
    static constexpr bool UseInterval = UseInterval_;

    AlgorithmValue<UseInterval> value;

    /// Checks if the value bounds are epsilon-close.
    [[nodiscard]]
    bool bounds_agree() const
    {
        static_assert(UseInterval, "No interval available!");
        return value.bounds_approximately_equal();
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
};

} // namespace probfd::algorithms::heuristic_search

#endif // PROBFD_ALGORITHMS_HEURISTIC_SEARCH_STATE_INFORMATION_H