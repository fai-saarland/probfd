#ifndef MDPS_ENGINES_HEURISTIC_SEARCH_STATE_INFORMATION_H
#define MDPS_ENGINES_HEURISTIC_SEARCH_STATE_INFORMATION_H

#include "probfd/types.h"
#include "probfd/value_type.h"
#include "probfd/value_utils.h"

#include <cassert>
#include <cstdint>

namespace probfd {
namespace engines {
namespace heuristic_search {

template <bool StoresPolicyT = false>
struct StatesPolicy {
};

template <>
struct StatesPolicy<true> {
    void set_policy(const ActionID& aid) { policy = aid; }

    ActionID get_policy() const { return policy; }

    ActionID policy = ActionID::undefined;
};

struct StateFlags {
    static constexpr uint8_t INITIALIZED = 1;
    static constexpr uint8_t DEAD = 2;
    static constexpr uint8_t GOAL = 4;
    static constexpr uint8_t FRINGE = 5;
    static constexpr uint8_t MASK = 7;
    static constexpr uint8_t BITS = 3;

    inline bool is_value_initialized() const { return (info & MASK) != 0; }
    inline bool is_dead_end() const { return (info & MASK) == DEAD; }
    inline bool is_goal_state() const { return (info & MASK) == GOAL; }
    inline bool is_terminal() const { return is_dead_end() || is_goal_state(); }
    inline bool is_on_fringe() const { return (info & MASK) == FRINGE; }

    inline void set_goal()
    {
        assert(!is_value_initialized());
        info = (info & ~MASK) | GOAL;
    }

    inline void set_on_fringe()
    {
        assert(!is_value_initialized());
        info = (info & ~MASK) | FRINGE;
    }

    inline void set_dead_end()
    {
        assert(!is_goal_state() && !is_dead_end());
        info = (info & ~MASK) | DEAD;
    }

    inline void removed_from_fringe()
    {
        assert(is_value_initialized() && !is_terminal());
        info = (info & ~MASK) | INITIALIZED;
    }

    uint8_t info = 0;
    value_type::value_t state_reward;
};

template <bool StoresPolicyT, bool IsInterval>
struct PerStateBaseInformation
    : public StatesPolicy<StoresPolicyT>
    , public StateFlags {
    static constexpr bool StoresPolicy = StoresPolicyT;
    static constexpr bool Interval = IsInterval;

    value_utils::IncumbentSolution<Interval> value;
};

} // namespace heuristic_search
} // namespace engines
} // namespace probfd

#endif // __HEURISTIC_SEARCH_STATE_INFORMATION_H__