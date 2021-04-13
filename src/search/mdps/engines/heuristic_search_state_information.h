#pragma once

#include "../evaluation_result.h"
#include "../types.h"
#include "../value_type.h"
#include "value_utils.h"

#include <cassert>
#include <cstdint>

namespace probabilistic {
namespace engines {
namespace heuristic_search {

struct StatesValue {
    inline const value_type::value_t& get_value() const { return value; }

    value_type::value_t value = 0;
};

template<typename HasTwoValues = std::false_type>
struct StatesValues : public StatesValue {
public:
    using DualBounds = std::false_type;

    void
    set_value(const value_utils::IncumbentSolution<std::false_type>& new_value)
    {
        value = new_value.first;
    }

    void set_value(const value_type::value_t& reward)
    {
        value = (value_type::value_t)reward;
    }

    bool update_value(
        const bool,
        const value_utils::IncumbentSolution<std::false_type>& new_value)
    {
        const bool result = !value_type::approx_equal()(value, new_value.first);
        value = new_value.first;
        return result;
    }

    bool bounds_equal() const { return true; }

    value_type::value_t error_bound() const { return value_type::zero; }
};

template<>
class StatesValues<std::true_type> : public StatesValue {
public:
    using DualBounds = std::true_type;

    void set_value(const value_type::value_t& reward)
    {
        value = (value_type::value_t)reward;
    }

    void
    set_value(const value_utils::IncumbentSolution<std::false_type>& new_value)
    {
        value = new_value.first;
        value2 = new_value.first;
    }

    void
    set_value(const value_utils::IncumbentSolution<std::true_type>& new_value)
    {
        value = new_value.first;
        value2 = new_value.first;
    }

    bool update_value(
        const bool x,
        const value_utils::IncumbentSolution<std::true_type>& new_value)
    {
        const bool result = !value_type::approx_equal()(value, new_value.first)
            || (x && !value_type::approx_equal()(value2, new_value.second));
        value = new_value.first;
        value2 = new_value.second;
        return result;
    }

    bool bounds_equal() const
    {
        return value_type::approx_equal()(value, value2);
    }

    value_type::value_t error_bound() const { return value - value2; }

    value_type::value_t value2;
};

template<typename StoresPolicyT = std::false_type>
struct StatesPolicy {
    using StoresPolicy = std::false_type;
    void set_policy(const ActionID&) const { }
    ActionID get_policy() const { return ActionID::undefined; }
};

template<>
struct StatesPolicy<std::true_type> {
    using StoresPolicy = std::true_type;

    StatesPolicy()
        : policy(ActionID::undefined)
    {
    }

    void set_policy(const ActionID& aid) { policy = aid; }

    ActionID get_policy() const { return policy; }

    ActionID policy;
};

struct StateFlags {
    static constexpr const uint8_t INITIALIZED = 1;
    static constexpr const uint8_t DEAD = 2;
    static constexpr const uint8_t RECOGNIZED_DEAD = 3;
    static constexpr const uint8_t GOAL = 4;
    static constexpr const uint8_t FRINGE = 5;
    static constexpr const uint8_t MASK = 7;
    static constexpr const uint8_t BITS = 3;

    inline bool is_value_initialized() const { return (MASK & info) != 0; }
    inline bool is_dead_end() const
    {
        const uint8_t masked = info & MASK;
        return masked == DEAD || masked == RECOGNIZED_DEAD;
    }
    inline bool is_recognized_dead_end() const
    {
        return (info & MASK) == RECOGNIZED_DEAD;
    }
    inline bool is_goal_state() const { return (MASK & info) == GOAL; }
    inline bool is_terminal() const { return is_dead_end() || is_goal_state(); }
    inline bool is_on_fringe() const { return (info & MASK) == FRINGE; }

    inline void set_goal()
    {
        assert(
            !is_value_initialized());
        info = (info & ~MASK) | GOAL;
    }

    inline void set_on_fringe()
    {
        assert(
            !is_value_initialized());
            info = (info & ~MASK) | FRINGE;
    }

    inline void set_dead_end()
    {
        assert(!is_goal_state()
            && !is_recognized_dead_end());
        info = (info & ~MASK) | DEAD;
    }

    inline void set_recognized_dead_end()
    {
        assert(!is_goal_state());
        info = (info & ~MASK) | RECOGNIZED_DEAD;
    }

    inline void removed_from_fringe()
    {
        assert(is_value_initialized() && !is_terminal());
        info = (info & ~MASK) | INITIALIZED;
    }

    inline value_type::value_t get_state_reward() const
    {
        assert(is_value_initialized());
        // return reward;
        return 0;
    }

    // value_type::value_t reward = 0;
    uint8_t info = 0;
};

template<typename StoresPolicyT, typename TwoValuesT>
struct PerStateBaseInformation : public StatesValues<TwoValuesT>,
                                 public StatesPolicy<StoresPolicyT>,
                                 public StateFlags {
    using StatesPolicy<StoresPolicyT>::StoresPolicy;
    using StatesValues<TwoValuesT>::DualBounds;

};

class PerStateInformationLookup {
public:
    virtual ~PerStateInformationLookup() = default;
    virtual const void* lookup(const StateID& state_id) = 0;
};

} // namespace heuristic_search
} // namespace engines
} // namespace probabilistic
