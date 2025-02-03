#ifndef PROBFD_ALIASES_H
#define PROBFD_ALIASES_H

#include "probfd/concepts.h"

#include <tuple>
#include <type_traits>

namespace probfd {

/// Typedef for the state value type
using value_t = double;

/**
 * @brief This type alias is declared as T if T models PassedByValue and
 * otherwise const T&.
 *
 * Whether a type should be passed by value in this context is defined by
 * the concept
 */
template <typename T>
using ParamType = typename std::conditional_t<PassedByValue<T>, T, const T&>;

/**
 * @brief Concatenated tuple type of a list of tuple types.
 */
template <typename... T>
using TupleCatType = decltype(std::tuple_cat(std::declval<T>()...));

} // namespace probfd

#endif // PROBFD_ALIASES_H
