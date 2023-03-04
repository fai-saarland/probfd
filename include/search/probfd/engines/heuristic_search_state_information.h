#ifndef PROBFD_ENGINES_HEURISTIC_SEARCH_STATE_INFORMATION_H
#define PROBFD_ENGINES_HEURISTIC_SEARCH_STATE_INFORMATION_H

#include "probfd/engines/utils.h"

#include "probfd/types.h"

#include <cassert>
#include <cstdint>

namespace probfd {
namespace engines {
namespace heuristic_search {

template <bool StorePolicy = false>
struct StatesPolicy {
};

template <>
struct StatesPolicy<true> {
    ActionID policy = ActionID::undefined;

    void set_policy(ActionID aid) { policy = aid; }
    ActionID get_policy() const { return policy; }
};

struct StateFlags {
    static constexpr uint8_t INITIALIZED = 1;
    static constexpr uint8_t DEAD = 2;
    static constexpr uint8_t GOAL = 4;
    static constexpr uint8_t FRINGE = 5;
    static constexpr uint8_t MASK = 7;
    static constexpr uint8_t BITS = 3;

    uint8_t info = 0;
    value_t state_cost;

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

template <bool StorePolicy_, bool UseInterval_>
struct PerStateBaseInformation
    : public StatesPolicy<StorePolicy_>
    , public StateFlags {
    static constexpr bool StorePolicy = StorePolicy_;
    static constexpr bool UseInterval = UseInterval_;

    EngineValueType<UseInterval> value;
};

} // namespace heuristic_search
} // namespace engines
} // namespace probfd

#endif // __HEURISTIC_SEARCH_STATE_INFORMATION_H__