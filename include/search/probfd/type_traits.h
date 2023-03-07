#ifndef PROBFD_CALL_TRAITS_H
#define PROBFD_CALL_TRAITS_H

#include <type_traits>

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

} // namespace probfd

#endif // __TYPES_H__