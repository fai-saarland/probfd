//
// Created by Thorsten Klößner on 03.02.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef PROBFD_ITEM_PROBABILITY_PAIR_H
#define PROBFD_ITEM_PROBABILITY_PAIR_H

#include "probfd/concepts.h"
#include "probfd/value_type.h"

#include <compare>
#include <type_traits>
#include <utility>

namespace probfd {

/// An item-probability pair.
template <typename T, typename PrType = value_t>
class ItemProbabilityPair {
    template <
        typename... Args,
        typename... Args2,
        size_t... Indices,
        size_t... Indices2>
    ItemProbabilityPair(
        std::tuple<Args...> args,
        std::tuple<Args2...> args2,
        std::index_sequence<Indices...>,
        std::index_sequence<Indices2...>)
        : item(std::get<Indices>(args)...)
        , probability(std::get<Indices2>(args2)...)
    {
    }

public:
    T item;             ///< The item.
    PrType probability; ///< The probability of the item.

    /// Pairs a default-constructed item with an indeterminate probability.
    ItemProbabilityPair()
        requires(std::is_default_constructible_v<T> &&
                 std::is_default_constructible_v<PrType>)
    = default;

    template <PairLike P>
        requires(std::is_constructible_v<T, std::tuple_element_t<0, P>> &&
                 std::is_constructible_v<PrType, std::tuple_element_t<1, P>>)
    explicit(
        !std::is_convertible_v<std::tuple_element_t<0, P>, T> ||
        !std::is_convertible_v<std::tuple_element_t<1, P>, T>)
        ItemProbabilityPair(P&& p)
        : item(std::get<0>(p))
        , probability(std::get<1>(p))
    {
    }

    /// Pairs a given item with a given probability.
    template <typename A, typename B>
        requires(std::is_constructible_v<T, A> &&
                 std::is_constructible_v<PrType, B>)
    explicit(!std::is_convertible_v<T, A> || !std::is_convertible_v<PrType, B>)
        ItemProbabilityPair(A&& item, B&& probability)
        : item(std::forward<A>(item))
        , probability(std::forward<B>(probability))
    {
    }

    /// Pairs an item constructed from a tuple of constructor arguments with a
    /// given probability.
    template <typename... Args, typename... Args2>
    ItemProbabilityPair(
        std::piecewise_construct_t,
        std::tuple<Args...> t1,
        std::tuple<Args2...> t2)
        : ItemProbabilityPair(
              t1,
              t2,
              std::index_sequence_for<Args...>{},
              std::index_sequence_for<Args2...>{})
    {
    }

    /// Lexicographical comparison.
    friend auto operator<=>(
        const ItemProbabilityPair<T, PrType>& left,
        const ItemProbabilityPair<T, PrType>& right) = default;

    template <typename A, typename B>
        requires(
            std::constructible_from<A, T> && std::constructible_from<B, PrType>)
    explicit(!std::is_convertible_v<T, A> || !std::is_convertible_v<PrType, B>)
    operator std::pair<A, B>() const
    {
        return std::pair<A, B>(item, probability);
    }

    template <typename A, typename B>
        requires(
            std::constructible_from<A, T> && std::constructible_from<B, PrType>)
    explicit(!std::is_convertible_v<T, A> || !std::is_convertible_v<PrType, B>)
    operator std::tuple<A, B>() const
    {
        return std::tuple<A, B>(item, probability);
    }

    template <std::size_t Index>
    auto& get() &
    {
        if constexpr (Index == 0) return item;
        if constexpr (Index == 1) return probability;
    }

    template <std::size_t Index>
    const auto& get() const&
    {
        if constexpr (Index == 0) return item;
        if constexpr (Index == 1) return probability;
    }

    template <std::size_t Index>
    auto&& get() &&
    {
        if constexpr (Index == 0) return std::move(item);
        if constexpr (Index == 1) return std::move(probability);
    }

    template <std::size_t Index>
    const auto&& get() const&&
    {
        if constexpr (Index == 0) return std::move(item);
        if constexpr (Index == 1) return std::move(probability);
    }
};

} // namespace probfd

template <typename T, typename F>
struct std::tuple_size<probfd::ItemProbabilityPair<T, F>>
    : public integral_constant<std::size_t, 2> {};

template <std::size_t I, typename T, typename F>
struct std::tuple_element<I, probfd::ItemProbabilityPair<T, F>> {
    static_assert(false, "Invalid index");
};

template <typename T, typename F>
struct std::tuple_element<0, probfd::ItemProbabilityPair<T, F>> {
    using type = T;
};

template <typename T, typename F>
struct std::tuple_element<1, probfd::ItemProbabilityPair<T, F>> {
    using type = F;
};

#endif // PROBFD_ITEM_PROBABILITY_PAIR_H
