#ifndef PROBFD_MDP_ALGORITHM_FWD_H
#define PROBFD_MDP_ALGORITHM_FWD_H

#include "probfd/concepts.h"

#include <concepts>

namespace probfd {

template <typename, typename>
class MDPAlgorithm;

namespace detail {
template <typename T>
    requires Specialization<T, MDPAlgorithm>
struct TypesOfS;

template <typename State, typename Action>
struct TypesOfS<MDPAlgorithm<State, Action>> {
    using state_type = State;
    using action_type = Action;
};

} // namespace detail

/// The state type used by a specialization of MDPAlgorithm.
template <DerivedFromSpecializationOf<MDPAlgorithm> T>
using StateTypeOf =
    typename detail::TypesOfS<typename T::MDPAlgorithm>::state_type;

/// The action type used by a specialization of MDPAlgorithm.
template <DerivedFromSpecializationOf<MDPAlgorithm> T>
using ActionTypeOf =
    typename detail::TypesOfS<typename T::MDPAlgorithm>::action_type;

} // namespace probfd

#endif