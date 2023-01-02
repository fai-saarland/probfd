#ifndef MDPS_QUOTIENT_SYSTEM_TYPES_H
#define MDPS_QUOTIENT_SYSTEM_TYPES_H

#include "probfd/types.h"

namespace probfd {

/// Namespace dedicated to the classes dealing with quotienting of an MDP.
namespace quotient_system {

template <typename Action>
struct QuotientAction {
    StateID state_id;
    ActionID action_id;

    explicit QuotientAction(const StateID& state_id, const ActionID& action_id)
        : state_id(state_id)
        , action_id(action_id)
    {
    }
};

template <typename T>
struct unwrap_action_type {
};

template <typename T>
struct unwrap_action_type<QuotientAction<T>> {
    using type = T;
};

} // namespace quotient_system
} // namespace probfd

#endif