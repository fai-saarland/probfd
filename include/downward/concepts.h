#ifndef DOWNWARD_CONCEPTS_H
#define DOWNWARD_CONCEPTS_H

#include <array>
#include <complex>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace downward {

namespace detail {
template <typename T>
struct IsTupleLikeHelper : std::false_type {};

template <typename T, std::size_t N>
struct IsTupleLikeHelper<std::array<T, N>> : std::true_type {};

template <typename T>
struct IsTupleLikeHelper<std::complex<T>> : std::true_type {};

template <typename A, typename B>
struct IsTupleLikeHelper<std::pair<A, B>> : std::true_type {};

template <typename... T>
struct IsTupleLikeHelper<std::tuple<T...>> : std::true_type {};

template <
    std::input_or_output_iterator I,
    std::sentinel_for<I> S,
    std::ranges::subrange_kind K>
struct IsTupleLikeHelper<std::ranges::subrange<I, S, K>> : std::true_type {};
} // namespace detail

/**
 * @brief Models the tuple-like concept from the C++ std library.
 *
 * See https://en.cppreference.com/w/cpp/utility/tuple/tuple-like.
 */
template <typename T>
concept TupleLike = detail::IsTupleLikeHelper<T>::value;

/**
 * @brief Models the pair-like concept from the C++ std library.
 *
 * See https://en.cppreference.com/w/cpp/utility/tuple/tuple-like.
 */
template <typename T>
concept PairLike = TupleLike<T> && std::tuple_size_v<T> == 2;

namespace detail {
template <typename T, template <typename...> typename U>
struct SpecializationHelper : std::false_type {};

template <typename... T, template <typename...> typename U>
struct SpecializationHelper<U<T...>, U> : std::true_type {};
} // namespace detail

/// This concept is satisfied if T is a specialization of the template U.
template <typename T, template <typename...> typename U>
concept Specialization = detail::SpecializationHelper<T, U>::value;

template <typename T, typename... List>
concept MemberOf = (std::is_same_v<T, List> || ...);

namespace detail {
template <typename... List>
struct dupl_free;

template <>
struct dupl_free<> : std::true_type {};

template <typename A, typename... Tail>
struct dupl_free<A, Tail...>
    : std::bool_constant<
          (!std::is_same_v<A, Tail> && ...) && dupl_free<Tail...>::value> {};
} // namespace detail

/// This concept is satisfied if all types are unique.
template <typename... List>
concept DuplicateFree = detail::dupl_free<List...>::value;

template <typename Fn, typename R, typename... Args>
concept InvocableRV = std::invocable<Fn, Args...> &&
                      std::convertible_to<std::invoke_result_t<Fn, Args...>, R>;

template <typename T, typename I>
concept Subscriptable = requires {
    { std::declval<T&>()[std::declval<I>()] };
};

template <typename T, typename I>
    requires Subscriptable<T, I>
using SubscriptResultType = decltype(std::declval<T&>()[std::declval<I>()]);

template <typename T, typename R, typename I>
concept SubscriptableRV =
    Subscriptable<T, I> && std::convertible_to<SubscriptResultType<T, I>, R>;

namespace detail {
template <template <class...> class Template, class... Args>
void DerivedFromSpecializationImpl(const Template<Args...>&);
}

/// This concept is satisfied if T has exactly one public base class which is a
/// specialization of U.
template <class T, template <class...> class U>
concept DerivedFromSpecializationOf =
    requires(const T& t) { detail::DerivedFromSpecializationImpl<U>(t); };

template <template <typename...> typename C, typename... T>
struct PartiallySpecialized {
    template <typename... U>
    struct type : C<T..., U...> {};
};

template <template <typename> typename TypePredicate, typename... Types>
concept AnyOf = (TypePredicate<Types>::value || ...);

template <template <typename> typename TypePredicate, typename... Types>
concept AllOf = (TypePredicate<Types>::value && ...);

} // namespace probfd

#endif
