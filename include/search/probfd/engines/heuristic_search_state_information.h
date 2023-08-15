#ifndef PROBFD_ENGINES_HEURISTIC_SEARCH_STATE_INFORMATION_H
#define PROBFD_ENGINES_HEURISTIC_SEARCH_STATE_INFORMATION_H

#include "probfd/engines/types.h"

#include "probfd/engine_interfaces/state_properties.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/types.h"

#include <cassert>
#include <cstdint>
#include <optional>

namespace probfd {
namespace engines {
namespace heuristic_search {

template <typename, bool StorePolicy = false>
struct StatesPolicy {};

template <typename Action>
struct StatesPolicy<Action, true> {
    std::optional<Action> policy = std::nullopt;

    void set_policy(Action a) { policy = a; }
    void clear_policy() { policy = std::nullopt; }
    std::optional<Action> get_policy() const { return policy; }
    bool update_policy(Action a)
    {
        if (!policy || a != policy) {
            set_policy(a);
            return true;
        }

        return false;
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

    bool is_value_initialized() const { return (info & MASK) != 0; }
    bool is_dead_end() const { return (info & MASK) == DEAD; }
    bool is_goal_state() const { return (info & MASK) == GOAL; }
    bool is_terminal() const { return is_dead_end() || is_goal_state(); }
    bool is_on_fringe() const { return (info & MASK) == FRINGE; }

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

    EngineValueType<UseInterval> value;

    /// Checks if the value bounds are epsilon-close.
    bool bounds_agree() const
    {
        static_assert(UseInterval, "No interval available!");
        return value.bounds_approximately_equal();
    }

    value_t get_value() const
    {
        if constexpr (UseInterval) {
            return value.lower;
        } else {
            return value;
        }
    }

    Interval get_bounds() const
    {
        if constexpr (UseInterval) {
            return value;
        } else {
            return Interval(value, INFINITE_VALUE);
        }
    }
};

} // namespace heuristic_search
} // namespace engines
} // namespace probfd

#endif // __HEURISTIC_SEARCH_STATE_INFORMATION_H__