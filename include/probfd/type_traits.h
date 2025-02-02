#ifndef PROBFD_TYPE_TRAITS_H
#define PROBFD_TYPE_TRAITS_H

#include <tuple>
#include <type_traits>
#include <utility>

// Forward Declarations
class OperatorID;

namespace probfd {

template <typename T>
struct is_cheap_to_copy : public std::bool_constant<std::is_scalar_v<T>> {};

template <>
struct is_cheap_to_copy<OperatorID> : public std::true_type {};

template <typename T>
static constexpr bool is_cheap_to_copy_v = is_cheap_to_copy<T>::value;

/**
 * @brief Alias template defining the best way to pass a parameter of a
 * given type.
 */
template <typename T>
using param_type =
    typename std::conditional_t<is_cheap_to_copy_v<T>, T, const T&>;

/**
 * @brief Concatenated tuple type of a list of tuple types.
 */
template <typename... T>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<T>()...));

} // namespace probfd

#endif // PROBFD_TYPE_TRAITS_H