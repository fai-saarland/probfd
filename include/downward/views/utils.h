#ifndef DOWNWARD_VIEWS_UTILS_H
#define DOWNWARD_VIEWS_UTILS_H

#include <ranges>
#include <type_traits>

namespace downward::views {
namespace detail {

// Alias for a type that is conditionally const.
template <bool Const, typename Tp>
using maybe_const_t = std::conditional_t<Const, const Tp, Tp>;

} // namespace detail

template <typename _Range>
concept simple_view =
    std::ranges::view<_Range> && std::ranges::range<const _Range> &&
    std::same_as<
        std::ranges::iterator_t<_Range>,
        std::ranges::iterator_t<const _Range>> &&
    std::same_as<
        std::ranges::sentinel_t<_Range>,
        std::ranges::sentinel_t<const _Range>>;

template <bool Const, typename... Vs>
concept all_random_access =
    (std::ranges::random_access_range<detail::maybe_const_t<Const, Vs>> && ...);

template <bool Const, typename... Vs>
concept all_bidirectional =
    (std::ranges::bidirectional_range<detail::maybe_const_t<Const, Vs>> && ...);

template <bool Const, typename... Vs>
concept all_forward =
    (std::ranges::forward_range<detail::maybe_const_t<Const, Vs>> && ...);

template <std::ranges::viewable_range _Rng>
using all_t = decltype(std::views::all(std::declval<_Rng>()));

} // namespace downward::views

#endif