#ifndef DOWNWARD_CARTESIAN_PRODUCT_H
#define DOWNWARD_CARTESIAN_PRODUCT_H

#include <version>

#ifdef __cpp_lib_ranges_cartesian_product

#include <ranges>

namespace downward::views {
inline constexpr auto cartesian_product = std::views::cartesian_product;
}

#else

#include "downward/views/utils.h"

#include <ranges>
#include <tuple>
#include <type_traits>

namespace downward::views {

template <std::ranges::viewable_range Range>
using all_t = decltype(std::views::all(std::declval<Range>()));

template <std::integral Tp>
constexpr auto to_unsigned_like(Tp t) noexcept
{
    return static_cast<std::make_unsigned_t<Tp>>(t);
}

template <typename Tp>
using make_unsigned_like_t = decltype(to_unsigned_like(std::declval<Tp>()));

template <typename Fp, typename Tuple>
constexpr auto tuple_transform(Fp&& f, Tuple&& tuple)
{
    return std::apply(
        [&]<typename... Ts>(Ts&&... elts) {
            return std::tuple<std::invoke_result_t<Fp&, Ts>...>(
                std::invoke(f, std::forward<Ts>(elts))...);
        },
        std::forward<Tuple>(tuple));
}

namespace detail {
template <bool Const, typename First, typename... Vs>
concept cartesian_product_is_random_access =
    (std::ranges::random_access_range<detail::maybe_const_t<Const, First>> && ... &&
     (std::ranges::random_access_range<detail::maybe_const_t<Const, Vs>> &&
      std::ranges::sized_range<detail::maybe_const_t<Const, Vs>>));

template <typename Range>
concept cartesian_product_common_arg =
    std::ranges::common_range<Range> ||
    (std::ranges::sized_range<Range> &&
     std::ranges::random_access_range<Range>);

template <bool Const, typename First, typename... Vs>
concept cartesian_product_is_bidirectional =
    (std::ranges::bidirectional_range<detail::maybe_const_t<Const, First>> && ... &&
     (std::ranges::bidirectional_range<detail::maybe_const_t<Const, Vs>> &&
      cartesian_product_common_arg<detail::maybe_const_t<Const, Vs>>));

template <typename First, typename... Vs>
concept cartesian_product_is_common = cartesian_product_common_arg<First>;

template <typename... Vs>
concept cartesian_product_is_sized = (std::ranges::sized_range<Vs> && ...);

template <
    bool Const,
    template <typename>
    class FirstSent,
    typename First,
    typename... Vs>
concept cartesian_is_sized_sentinel =
    (std::sized_sentinel_for<
         FirstSent<detail::maybe_const_t<Const, First>>,
         std::ranges::iterator_t<detail::maybe_const_t<Const, First>>> &&
     ... &&
     (std::ranges::sized_range<detail::maybe_const_t<Const, Vs>> &&
      std::sized_sentinel_for<
          std::ranges::iterator_t<detail::maybe_const_t<Const, Vs>>,
          std::ranges::iterator_t<detail::maybe_const_t<Const, Vs>>>));

template <cartesian_product_common_arg _Range>
constexpr auto cartesian_common_arg_end(_Range& __r)
{
    if constexpr (std::ranges::common_range<_Range>)
        return std::ranges::end(__r);
    else
        return std::ranges::begin(__r) + std::ranges::distance(__r);
}
} // namespace detail

template <std::ranges::input_range First, std::ranges::forward_range... Vs>
    requires(std::ranges::view<First> && ... && std::ranges::view<Vs>)
class cartesian_product_view
    : public std::ranges::view_interface<cartesian_product_view<First, Vs...>> {
    std::tuple<First, Vs...> _M_bases;

    template <bool>
    class Iterator;

    static auto S_difference_type()
    {
        // TODO: Implement the recommended practice of using the smallest
        // sufficiently wide type according to the maximum sizes of the
        // underlying ranges?
        return std::common_type_t<
            ptrdiff_t,
            std::ranges::range_difference_t<First>,
            std::ranges::range_difference_t<Vs>...>{};
    }

public:
    cartesian_product_view() = default;

    constexpr explicit cartesian_product_view(First first, Vs... rest)
        : _M_bases(std::move(first), std::move(rest)...)
    {
    }

    constexpr Iterator<false> begin()
        requires(!simple_view<First> || ... || !simple_view<Vs>)
    {
        return Iterator<false>(
            *this,
            tuple_transform(std::ranges::begin, _M_bases));
    }

    constexpr Iterator<true> begin() const
        requires(
            std::ranges::range<const First> && ... &&
            std::ranges::range<const Vs>)
    {
        return Iterator<true>(
            *this,
            tuple_transform(std::ranges::begin, _M_bases));
    }

    constexpr Iterator<false> end()
        requires(
            (!simple_view<First> || ... || !simple_view<Vs>) &&
            detail::cartesian_product_is_common<First, Vs...>)
    {
        auto its = [this]<size_t... Is>(std::index_sequence<Is...>) {
            using Ret = std::tuple<
                std::ranges::iterator_t<First>,
                std::ranges::iterator_t<Vs>...>;
            bool empty_tail =
                (std::ranges::empty(std::get<1 + Is>(_M_bases)) || ...);
            auto& first = std::get<0>(_M_bases);
            return Ret{
                (empty_tail ? std::ranges::begin(first)
                            : detail::cartesian_common_arg_end(first)),
                std::ranges::begin(std::get<1 + Is>(_M_bases))...};
        }(std::make_index_sequence<sizeof...(Vs)>{});

        return Iterator<false>{*this, std::move(its)};
    }

    constexpr Iterator<true> end() const
        requires detail::cartesian_product_is_common<const First, const Vs...>
    {
        auto its = [this]<size_t... Is>(std::index_sequence<Is...>) {
            using Ret = std::tuple<
                std::ranges::iterator_t<const First>,
                std::ranges::iterator_t<const Vs>...>;
            bool empty_tail =
                (std::ranges::empty(std::get<1 + Is>(_M_bases)) || ...);
            auto& first = std::get<0>(_M_bases);
            return Ret{
                (empty_tail ? std::ranges::begin(first)
                            : detail::cartesian_common_arg_end(first)),
                std::ranges::begin(std::get<1 + Is>(_M_bases))...};
        }(std::make_index_sequence<sizeof...(Vs)>{});

        return Iterator<true>(*this, std::move(its));
    }

    constexpr std::default_sentinel_t end() const noexcept
    {
        return std::default_sentinel;
    }

    constexpr auto size()
        requires detail::cartesian_product_is_sized<First, Vs...>
    {
        using ST = make_unsigned_like_t<decltype(S_difference_type())>;
        return [&]<size_t... Is>(std::index_sequence<Is...>) {
            return (
                static_cast<ST>(std::ranges::size(std::get<Is>(_M_bases))) *
                ...);
        }(std::make_index_sequence<1 + sizeof...(Vs)>{});
    }

    constexpr auto size() const
        requires detail::cartesian_product_is_sized<const First, const Vs...>
    {
        using ST = make_unsigned_like_t<decltype(S_difference_type())>;
        return [&]<size_t... Is>(std::index_sequence<Is...>) {
            return (
                static_cast<ST>(std::ranges::size(std::get<Is>(_M_bases))) *
                ...);
        }(std::make_index_sequence<1 + sizeof...(Vs)>{});
    }
};

template <typename... Vs>
cartesian_product_view(Vs&&...)
    -> cartesian_product_view<std::ranges::views::all_t<Vs>...>;

template <std::ranges::input_range First, std::ranges::forward_range... Vs>
    requires(std::ranges::view<First> && ... && std::ranges::view<Vs>)
template <bool Const>
class cartesian_product_view<First, Vs...>::Iterator {
    using Parent = detail::maybe_const_t<Const, cartesian_product_view>;
    Parent* _M_parent = nullptr;
    std::tuple<
        std::ranges::iterator_t<detail::maybe_const_t<Const, First>>,
        std::ranges::iterator_t<detail::maybe_const_t<Const, Vs>>...>
        _M_current;

    constexpr Iterator(Parent& parent, decltype(_M_current) current)
        : _M_parent(std::addressof(parent))
        , _M_current(std::move(current))
    {
    }

    static auto S_iter_concept()
    {
        if constexpr (detail::cartesian_product_is_random_access<
                          Const,
                          First,
                          Vs...>)
            return std::random_access_iterator_tag{};
        else if constexpr (detail::cartesian_product_is_bidirectional<
                               Const,
                               First,
                               Vs...>)
            return std::bidirectional_iterator_tag{};
        else if constexpr (std::ranges::forward_range<
                               detail::maybe_const_t<Const, First>>)
            return std::forward_iterator_tag{};
        else
            return std::input_iterator_tag{};
    }

    friend cartesian_product_view;

public:
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = decltype(S_iter_concept());
    using value_type = std::tuple<
        std::ranges::range_value_t<detail::maybe_const_t<Const, First>>,
        std::ranges::range_value_t<detail::maybe_const_t<Const, Vs>>...>;
    using reference = std::tuple<
        std::ranges::range_reference_t<detail::maybe_const_t<Const, First>>,
        std::ranges::range_reference_t<detail::maybe_const_t<Const, Vs>>...>;
    using difference_type =
        decltype(cartesian_product_view::S_difference_type());

    Iterator() = default;

    constexpr Iterator(Iterator<!Const> i)
        requires Const && (std::convertible_to<
                               std::ranges::iterator_t<First>,
                               std::ranges::iterator_t<const First>> &&
                           ... &&
                           std::convertible_to<
                               std::ranges::iterator_t<Vs>,
                               std::ranges::iterator_t<const Vs>>)
        : _M_parent(std::addressof(i._M_parent))
        , _M_current(std::move(i._M_current))
    {
    }

    constexpr auto operator*() const
    {
        auto f = [](auto& i) -> decltype(auto) { return *i; };
        return tuple_transform(f, _M_current);
    }

    constexpr Iterator& operator++()
    {
        M_next();
        return *this;
    }

    constexpr void operator++(int) { ++*this; }

    constexpr Iterator operator++(int)
        requires std::ranges::forward_range<detail::maybe_const_t<Const, First>>
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr Iterator& operator--()
        requires detail::cartesian_product_is_bidirectional<Const, First, Vs...>
    {
        M_prev();
        return *this;
    }

    constexpr Iterator operator--(int)
        requires detail::cartesian_product_is_bidirectional<Const, First, Vs...>
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr Iterator& operator+=(difference_type x)
        requires detail::cartesian_product_is_random_access<Const, First, Vs...>
    {
        M_advance(x);
        return *this;
    }

    constexpr Iterator& operator-=(difference_type x)
        requires detail::cartesian_product_is_random_access<Const, First, Vs...>
    {
        return *this += -x;
    }

    constexpr reference operator[](difference_type n) const
        requires detail::cartesian_product_is_random_access<Const, First, Vs...>
    {
        return *((*this) + n);
    }

    friend constexpr bool operator==(const Iterator& x, const Iterator& y)
        requires std::equality_comparable<
            std::ranges::iterator_t<detail::maybe_const_t<Const, First>>>
    {
        return x._M_current == y._M_current;
    }

    friend constexpr bool operator==(const Iterator& x, std::default_sentinel_t)
    {
        return [&]<size_t... Is>(std::index_sequence<Is...>) {
            return (
                (std::get<Is>(x._M_current) ==
                 std::ranges::end(std::get<Is>(x._M_parent->_M_bases))) ||
                ...);
        }(std::make_index_sequence<1 + sizeof...(Vs)>{});
    }

    friend constexpr auto operator<=>(const Iterator& x, const Iterator& y)
        requires all_random_access<Const, First, Vs...>
    {
        return x._M_current <=> y._M_current;
    }

    friend constexpr Iterator operator+(Iterator x, difference_type y)
        requires detail::cartesian_product_is_random_access<Const, First, Vs...>
    {
        return x += y;
    }

    friend constexpr Iterator operator+(difference_type x, Iterator y)
        requires detail::cartesian_product_is_random_access<Const, First, Vs...>
    {
        return y += x;
    }

    friend constexpr Iterator operator-(Iterator x, difference_type y)
        requires detail::cartesian_product_is_random_access<Const, First, Vs...>
    {
        return x -= y;
    }

    friend constexpr difference_type
    operator-(const Iterator& x, const Iterator& y)
        requires detail::cartesian_is_sized_sentinel<
            Const,
            std::ranges::iterator_t,
            First,
            Vs...>
    {
        return x.M_distance_from(y._M_current);
    }

    friend constexpr difference_type
    operator-(const Iterator& i, std::default_sentinel_t)
        requires detail::cartesian_is_sized_sentinel<
            Const,
            std::ranges::sentinel_t,
            First,
            Vs...>
    {
        std::tuple end_tuple = [&]<size_t... Is>(std::index_sequence<Is...>) {
            return std::tuple{
                std::ranges::end(std::get<0>(i._M_parent->_M_bases)),
                std::ranges::begin(std::get<1 + Is>(i._M_parent->_M_bases))...};
        }(std::make_index_sequence<sizeof...(Vs)>{});
        return i.M_distance_from(end_tuple);
    }

    friend constexpr difference_type
    operator-(std::default_sentinel_t, const Iterator& i)
        requires detail::cartesian_is_sized_sentinel<
            Const,
            std::ranges::sentinel_t,
            First,
            Vs...>
    {
        return -(i - std::default_sentinel);
    }

    friend constexpr auto iter_move(const Iterator& i)
    {
        return tuple_transform(std::ranges::iter_move, i._M_current);
    }

    friend constexpr void iter_swap(const Iterator& l, const Iterator& r)
        requires(
            std::indirectly_swappable<
                std::ranges::iterator_t<detail::maybe_const_t<Const, First>>> &&
            ... &&
            std::indirectly_swappable<
                std::ranges::iterator_t<detail::maybe_const_t<Const, Vs>>>)
    {
        [&]<size_t... Is>(std::index_sequence<Is...>) {
            (std::ranges::iter_swap(
                 std::get<Is>(l._M_current),
                 std::get<Is>(r._M_current)),
             ...);
        }(std::make_index_sequence<1 + sizeof...(Vs)>{});
    }

private:
    template <size_t Nm = sizeof...(Vs)>
    constexpr void M_next()
    {
        auto& it = std::get<Nm>(_M_current);
        ++it;
        if constexpr (Nm > 0)
            if (it == std::ranges::end(std::get<Nm>(_M_parent->_M_bases))) {
                it = std::ranges::begin(std::get<Nm>(_M_parent->_M_bases));
                M_next<Nm - 1>();
            }
    }

    template <size_t Nm = sizeof...(Vs)>
    constexpr void M_prev()
    {
        auto& it = std::get<Nm>(_M_current);
        if constexpr (Nm > 0)
            if (it == std::ranges::begin(std::get<Nm>(_M_parent->_M_bases))) {
                it = detail::cartesian_common_arg_end(
                    std::get<Nm>(_M_parent->_M_bases));
                M_prev<Nm - 1>();
            }
        --it;
    }

    template <size_t Nm = sizeof...(Vs)>
    constexpr void M_advance(difference_type x)
        requires detail::cartesian_product_is_random_access<Const, First, Vs...>
    {
        if (x == 1)
            M_next<Nm>();
        else if (x == -1)
            M_prev<Nm>();
        else if (x != 0) {
            // Constant time iterator advancement.
            auto& r = std::get<Nm>(_M_parent->_M_bases);
            auto& it = std::get<Nm>(_M_current);
            if constexpr (Nm == 0) {
                it += x;
            } else {
                auto size = std::ranges::ssize(r);
                auto begin = std::ranges::begin(r);
                auto offset = it - begin;
                offset += x;
                x = offset / size;
                offset %= size;
                if (offset < 0) {
                    offset = size + offset;
                    --x;
                }
                it = begin + offset;
                M_advance<Nm - 1>(x);
            }
        }
    }

    template <typename Tuple>
    constexpr difference_type M_distance_from(const Tuple& t) const
    {
        return [&]<size_t... Is>(std::index_sequence<Is...>) {
            return (M_scaled_distance<Is>(t) + ...);
        }(std::make_index_sequence<1 + sizeof...(Vs)>{});
    }

    template <size_t Nm, typename Tuple>
    constexpr difference_type M_scaled_distance(const Tuple& t) const
    {
        auto dist = static_cast<difference_type>(
            std::get<Nm>(_M_current) - std::get<Nm>(t));
        dist *= M_scaled_size<Nm + 1>();
        return dist;
    }

    template <size_t Nm>
    constexpr difference_type M_scaled_size() const
    {
        if constexpr (Nm <= sizeof...(Vs)) {
            auto size = static_cast<difference_type>(
                std::ranges::size(std::get<Nm>(_M_parent->_M_bases)));
            size *= M_scaled_size<Nm + 1>();
            return size;
        } else
            return static_cast<difference_type>(1);
    }
};

namespace detail {
template <typename... Ts>
concept can_cartesian_product_view =
    requires { cartesian_product_view<all_t<Ts>...>(std::declval<Ts>()...); };

struct CartesianProduct
    : public std::ranges::range_adaptor_closure<CartesianProduct> {
    template <typename... Ts>
        requires(
            sizeof...(Ts) == 0 || detail::can_cartesian_product_view<Ts...>)
    constexpr auto operator() [[nodiscard]] (Ts&&... ts) const
    {
        if constexpr (sizeof...(Ts) == 0)
            return std::ranges::views::single(std::tuple{});
        else
            return cartesian_product_view<all_t<Ts>...>(
                std::forward<Ts>(ts)...);
    }
};

} // namespace detail

inline constexpr detail::CartesianProduct cartesian_product;

} // namespace downward::views

#endif

#endif // downward_CARTESIAN_PRODUCT_H
