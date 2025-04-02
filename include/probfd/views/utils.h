#ifndef PROBFD_VIEWS_UTILS_H
#define PROBFD_VIEWS_UTILS_H

#include <type_traits>

namespace probfd::views {
namespace detail {

// Alias for a type that is conditionally const.
template <bool Const, typename Tp>
using maybe_const_t = std::conditional_t<Const, const Tp, Tp>;

}

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

}

#endif