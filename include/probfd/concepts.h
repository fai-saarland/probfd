//
// Created by Thorsten Klößner on 30.01.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef PROBFD_CONCEPTS_H
#define PROBFD_CONCEPTS_H

#include <array>
#include <complex>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace probfd {

namespace detail {
template <typename T>
struct is_tuple_like_helper : std::false_type {};

template <typename T, std::size_t N>
struct is_tuple_like_helper<std::array<T, N>> : std::true_type {};

template <typename T>
struct is_tuple_like_helper<std::complex<T>> : std::true_type {};

template <typename A, typename B>
struct is_tuple_like_helper<std::pair<A, B>> : std::true_type {};

template <typename... T>
struct is_tuple_like_helper<std::tuple<T...>> : std::true_type {};

template <
    std::input_or_output_iterator I,
    std::sentinel_for<I> S,
    std::ranges::subrange_kind K>
struct is_tuple_like_helper<std::ranges::subrange<I, S, K>> : std::true_type {};
} // namespace detail

/**
 * @brief Models the tuple-like concept from the C++ std library.
 *
 * See https://en.cppreference.com/w/cpp/utility/tuple/tuple-like.
 */
template <typename T>
concept TupleLike = detail::is_tuple_like_helper<T>::value;

/**
 * @brief Models the pair-like concept from the C++ std library.
 *
 * See https://en.cppreference.com/w/cpp/utility/tuple/tuple-like.
 */
template <typename T>
concept PairLike = TupleLike<T> && std::tuple_size_v<T> == 2;

} // namespace probfd

#endif // PROBFD_CONCEPTS_H
