#ifndef DOWNWARD_TUPLE_UTILS_H
#define DOWNWARD_TUPLE_UTILS_H

#include "downward/concepts.h"

#include <concepts>
#include <memory>
#include <optional>
#include <utility>

namespace downward {

template <typename T, typename TupleLike>
constexpr auto&& get_shared(TupleLike&& t)
{
    return std::get<std::shared_ptr<T>>(std::forward<TupleLike>(t));
}

template <typename T, typename TupleLike>
constexpr auto&& get_unique(TupleLike&& t)
{
    return std::get<std::unique_ptr<T>>(std::forward<TupleLike>(t));
}

template <typename... T, typename TupleLike>
constexpr auto slice_shared(TupleLike&& t)
{
    return std::forward_as_tuple(get_shared<T>(std::forward<TupleLike>(t))...);
}

template <typename... T, typename TupleLike>
constexpr auto slice_unique(TupleLike&& t)
{
    return std::forward_as_tuple(get_unique<T>(std::forward<TupleLike>(t))...);
}

template <typename... T, typename TupleLike>
constexpr std::tuple<T...> slice(TupleLike&& t)
{
    return std::forward_as_tuple(std::get<T>(std::forward<TupleLike>(t))...);
}

namespace detail {

template <std::integral I, I... ints, I... ints2>
constexpr std::integer_sequence<I, ints..., ints2...> concat_sequences(
    std::integer_sequence<I, ints...> = {},
    std::integer_sequence<I, ints2...> = {})
{
    return std::integer_sequence<I, ints..., ints2...>{};
}

template <std::size_t n, std::integral I, I first, I... ints>
constexpr auto nth(std::integer_sequence<I, first, ints...> = {})
    requires(n < sizeof...(ints) + 1)
{
    if constexpr (n == 0) {
        return first;
    } else {
        return concat_sequences<n - 1>(std::integer_sequence<I, ints...>{});
    }
}

template <std::size_t n, std::integral I>
constexpr auto nth(std::integer_sequence<I>) = delete;

template <
    std::size_t i,
    template <typename, typename> typename C,
    typename T,
    typename... U>
struct occurences;

template <std::size_t i, template <typename, typename> typename C, typename T>
struct occurences<i, C, T> {
    static constexpr std::index_sequence<> value;
    using type = decltype(value);
};

template <
    std::size_t i,
    template <typename, typename> typename C,
    typename T,
    typename S,
    typename... U>
struct occurences<i, C, T, S, U...> {
    static constexpr auto select()
    {
        if constexpr (C<S, T>::value) {
            return concat_sequences(std::index_sequence<i>{}, rec);
        } else {
            return rec;
        }
    }

    static constexpr auto rec = occurences<i + 1, C, T, U...>::value;
    static constexpr auto value = select();
    using type = decltype(value);
};

template <template <typename, typename> typename C, typename T, typename... U>
constexpr std::size_t num_matches = occurences<0, C, T, U...>::value.size();

template <template <typename, typename> typename C, typename T, typename... U>
constexpr std::size_t has_match = occurences<0, C, T, U...>::value.size() != 0;

template <
    template <typename, typename> typename C,
    std::size_t i,
    typename T,
    typename... U>
    requires(i < num_matches<C, T, U...>)
constexpr std::size_t get_match_idx = nth<i>(occurences<0, C, T, U...>::value);

template <template <typename, typename> typename C, typename T, typename... U>
    requires(num_matches<C, T, U...> > 0)
constexpr std::size_t get_first_match_idx =
    nth<0>(occurences<0, C, T, U...>::value);

template <typename T, typename... U>
constexpr std::size_t num_occurences = num_matches<std::is_same, T, U...>;

template <std::size_t i, typename T, typename... U>
    requires(i < num_occurences<T, U...>)
constexpr std::size_t get_occurence_idx =
    get_match_idx<std::is_same, i, T, U...>;

template <typename T, typename... U>
    requires(num_occurences<T, U...> == 1)
constexpr std::size_t type_index_v = get_occurence_idx<0, T, U...>;

template <typename S, typename T>
struct inv_is_convertible : std::is_convertible<T, S> {};

template <typename S, typename T>
struct is_constructible : std::is_constructible<S, T> {};

template <typename S, typename T>
struct inv_is_constructible : std::is_constructible<T, S> {};

template <template <typename, typename> typename C, typename U, typename... T>
constexpr bool any_of = (C<U, T>::value || ...);

template <template <typename, typename> typename C, typename U, typename... T>
constexpr bool all_of = (C<U, T>::value && ...);

template <typename... U, typename TupleLike, std::size_t... indices>
constexpr auto replace(std::index_sequence<indices...>, TupleLike&& t, U&&... u)
{
    static_assert((
        any_of<
            inv_is_constructible,
            U,
            std::tuple_element_t<indices, std::remove_cvref_t<TupleLike>>...> &&
        ...));

    using R = std::remove_cvref_t<TupleLike>;

    constexpr auto get_el = []<std::size_t t_index>(
                                std::index_sequence<t_index>,
                                TupleLike&& mt,
                                U&&... mu) -> decltype(auto) {
        using S = std::tuple_element_t<t_index, R>;

        if constexpr (has_match<is_constructible, S, U...>) {
            constexpr std::optional<std::size_t> index =
                get_first_match_idx<is_constructible, S, U...>;
            return std::get<index>(std::forward_as_tuple(mu...));
        } else {
            return std::get<S>(std::forward<TupleLike>(mt));
        }
    };

    return R(get_el(
        std::index_sequence<indices>{},
        std::forward<TupleLike>(t),
        std::forward<U>(u)...)...);
}

} // namespace detail

template <typename... U, typename TupleLike>
constexpr auto replace(TupleLike&& t, U&&... u)
{
    return detail::replace(
        std::make_index_sequence<
            std::tuple_size_v<std::remove_cvref_t<TupleLike>>>{},
        std::forward<TupleLike>(t),
        std::forward<U>(u)...);
}

template <typename TupleLike, typename F>
constexpr auto map_tuple(TupleLike&& t, F&& f)
{
    return std::apply(
        [f = std::forward<F>(f)](auto&&... x) {
            return std::forward_as_tuple(f(x)...);
        },
        t);
}

template <auto F, typename TupleLike>
constexpr auto map(TupleLike&& t)
{
    return std::apply(
        [](auto&&... x) { return std::forward_as_tuple(F(x)...); },
        t);
}

template <typename TupleLike>
constexpr auto to_refs(TupleLike&& t)
{
    return std::apply(
        [](auto&&... x) { return std::forward_as_tuple(*x...); },
        std::forward<TupleLike>(t));
}

template <typename TupleLike>
constexpr auto to_array(TupleLike&& tuple)
{
    constexpr auto get_array = []<typename... T>(T&&... x) {
        return std::array{std::forward<T>(x)...};
    };
    return std::apply(get_array, std::forward<TupleLike>(tuple));
}

} // namespace downward

#endif
