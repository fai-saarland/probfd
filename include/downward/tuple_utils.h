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

template <template <typename...> typename C, typename... T>
struct partially_specialized {
    template <typename... U>
    struct type : C<T..., U...> {};
};

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
        return nth<n - 1>(std::integer_sequence<I, ints...>{});
    }
}

template <std::size_t n, std::integral I>
constexpr auto nth(std::integer_sequence<I>) = delete;

template <std::size_t i, template <typename> typename C, typename... U>
struct occurences;

template <std::size_t i, template <typename> typename C>
struct occurences<i, C> {
    static constexpr std::index_sequence<> value = {};
    using type = decltype(value);
};

template <
    std::size_t i,
    template <typename> typename C,
    typename S,
    typename... U>
struct occurences<i, C, S, U...> {
    static constexpr auto select()
    {
        if constexpr (C<S>::value) {
            return concat_sequences(std::index_sequence<i>{}, rec);
        } else {
            return rec;
        }
    }

    static constexpr auto rec = occurences<i + 1, C, U...>::value;
    static constexpr auto value = select();
    using type = decltype(value);
};

template <template <typename> typename C, typename... U>
constexpr std::size_t num_matches = occurences<0, C, U...>::value.size();

template <template <typename> typename C, typename... U>
constexpr std::size_t has_match = occurences<0, C, U...>::value.size() != 0;

template <template <typename> typename C, std::size_t i, typename... U>
    requires(i < num_matches<C, U...>)
constexpr std::size_t get_match_idx = nth<i>(occurences<0, C, U...>::value);

template <template <typename> typename C, typename... U>
    requires(num_matches<C, U...> > 0)
constexpr std::size_t get_first_match_idx =
    nth<0>(occurences<0, C, U...>::value);

template <typename T, typename... U>
constexpr std::size_t num_occurences =
    num_matches<partially_specialized<std::is_same, T>::template type, U...>;

template <std::size_t i, typename T, typename... U>
    requires(i < num_occurences<T, U...>)
constexpr std::size_t get_occurence_idx = get_match_idx<
    partially_specialized<std::is_same, T>::template type,
    i,
    U...>;

template <typename T, typename... U>
    requires(num_occurences<T, U...> == 1)
constexpr std::size_t type_index_v = get_occurence_idx<0, T, U...>;

template <typename S, typename T>
struct inv_is_convertible : std::is_convertible<T, S> {};

template <typename S, typename T>
struct is_constructible : std::is_constructible<S, T> {};

template <typename S, typename T>
struct inv_is_constructible : std::is_constructible<T, S> {};

template <template <typename> typename TypePredicate, typename... Types>
constexpr bool any_of = (TypePredicate<Types>::value || ...);

template <template <typename> typename TypePredicate, typename... Types>
constexpr bool all_of = (TypePredicate<Types>::value && ...);

template <std::size_t t_index, typename TupleLike, typename... U>
constexpr auto get_el(TupleLike&& mt, U&&... mu) -> decltype(auto)
{
    using R = typename std::remove_cvref_t<TupleLike>;
    using S = std::tuple_element_t<t_index, R>;

    if constexpr (has_match<
                      partially_specialized<std::is_constructible, S>::
                          template type,
                      U...>) {
        constexpr std::size_t index = get_first_match_idx<
            partially_specialized<std::is_constructible, S>::template type,
            U...>;
        return std::get<index>(std::forward_as_tuple(mu...));
    } else {
        return std::get<S>(std::forward<TupleLike>(mt));
    }
}

template <typename... U, typename TupleLike, std::size_t... indices>
constexpr auto replace(std::index_sequence<indices...>, TupleLike&& t, U&&... u)
{
    using R = std::remove_cvref_t<TupleLike>;

    static_assert(
        (any_of<
             partially_specialized<inv_is_constructible, U>::template type,
             std::tuple_element_t<indices, R>...> &&
         ...));

    return R(
        get_el<indices>(std::forward<TupleLike>(t), std::forward<U>(u)...)...);
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
        std::forward<TupleLike>(t));
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
