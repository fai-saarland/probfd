#ifndef PROBFD_CONCAT_H
#define PROBFD_CONCAT_H

#include <ranges>

namespace probfd::views {

#ifndef __cpp_lib_ranges_concat

#include <iterator>
#include <utility>
#include <variant>

namespace detail {

// Alias for a type that is conditionally const.
template <bool Const, typename Tp>
using maybe_const_t = std::conditional_t<Const, const Tp, Tp>;

template <typename... Rs>
using concat_reference_t =
    std::common_reference_t<std::ranges::range_reference_t<Rs>...>;

template <typename... Rs>
using concat_value_t = std::common_type_t<std::ranges::range_value_t<Rs>...>;

template <typename... Rs>
using concat_rvalue_reference_t =
    std::common_reference_t<std::ranges::range_rvalue_reference_t<Rs>...>;

template <typename Ref, typename RRef, typename It>
concept concat_indirectly_readable_impl = requires(const It it) {
    { *it } -> std::convertible_to<Ref>;
    { std::ranges::iter_move(it) } -> std::convertible_to<RRef>;
};

template <typename... Rs>
concept concat_indirectly_readable = std::common_reference_with<
                                         concat_reference_t<Rs...>&&,
                                         concat_value_t<Rs...>&> &&
                                     std::common_reference_with<
                                         concat_reference_t<Rs...>&&,
                                         concat_rvalue_reference_t<Rs...>&&> &&
                                     std::common_reference_with<
                                         concat_rvalue_reference_t<Rs...>&&,
                                         concat_value_t<Rs...> const&> &&
                                     (concat_indirectly_readable_impl<
                                          concat_reference_t<Rs...>,
                                          concat_rvalue_reference_t<Rs...>,
                                          std::ranges::iterator_t<Rs>> &&
                                      ...);

template <typename... Rs>
concept concatable = requires {
    typename concat_reference_t<Rs...>;
    typename concat_value_t<Rs...>;
    typename concat_rvalue_reference_t<Rs...>;
} && concat_indirectly_readable<Rs...>;

template <bool Const, typename Range, typename... Rs>
struct all_but_last_common {
    static inline constexpr bool value = requires {
        requires(
            std::ranges::common_range<maybe_const_t<Const, Range>> &&
            all_but_last_common<Const, Rs...>::value);
    };
};

template <bool Const, typename Range>
struct all_but_last_common<Const, Range> {
    static inline constexpr bool value = true;
};

template <bool Const, typename... Vs>
concept all_random_access =
    (std::ranges::random_access_range<maybe_const_t<Const, Vs>> && ...);

template <bool Const, typename... Vs>
concept all_bidirectional =
    (std::ranges::bidirectional_range<maybe_const_t<Const, Vs>> && ...);

template <bool Const, typename... Vs>
concept all_forward =
    (std::ranges::forward_range<maybe_const_t<Const, Vs>> && ...);

template <bool Const, typename... Rs>
concept concat_is_random_access =
    all_random_access<Const, Rs...> && all_but_last_common<Const, Rs...>::value;

template <bool Const, typename... Rs>
concept concat_is_bidirectional =
    all_bidirectional<Const, Rs...> && all_but_last_common<Const, Rs...>::value;

template <typename Range, typename... Rs>
struct last_is_common {
    static inline constexpr bool value = last_is_common<Rs...>::value;
};

template <typename Range>
struct last_is_common<Range> {
    static inline constexpr bool value = std::ranges::common_range<Range>;
};

template <typename Fp, typename Tuple>
constexpr auto tuple_transform(Fp&& f, Tuple&& t)
{
    return std::apply(
        [&]<typename... Ts>(Ts&&... elts) {
            return tuple<std::invoke_result_t<Fp&, Ts>...>(
                std::invoke(f, std::forward<Ts>(elts))...);
        },
        std::forward<Tuple>(t));
}

template <typename Range>
concept simple_view =
    std::ranges::view<Range> && std::ranges::range<const Range> &&
    std::same_as<
        std::ranges::iterator_t<Range>,
        std::ranges::iterator_t<const Range>> &&
    std::same_as<
        std::ranges::sentinel_t<Range>,
        std::ranges::sentinel_t<const Range>>;

template <std::integral Tp>
constexpr auto to_unsigned_like(Tp t) noexcept
{
    return static_cast<std::make_unsigned_t<Tp>>(t);
}

template <typename Tp>
using make_unsigned_like_t =
    decltype(detail::to_unsigned_like(std::declval<Tp>()));

} // namespace detail

using detail::maybe_const_t;

template <std::ranges::input_range... Vs>
    requires(std::ranges::view<Vs> && ...) &&
            (sizeof...(Vs) > 0) && detail::concatable<Vs...>
class concat_view : public std::ranges::view_interface<concat_view<Vs...>> {
    std::tuple<Vs...> M_views;

    template <bool Const>
    class iterator;

public:
    constexpr concat_view() = default;

    constexpr explicit concat_view(Vs... views)
        : M_views(std::move(views)...)
    {
    }

    constexpr iterator<false> begin()
        requires(!(detail::simple_view<Vs> && ...))
    {
        iterator<false> it(
            this,
            std::in_place_index<0>,
            std::ranges::begin(std::get<0>(M_views)));
        it.template M_satisfy<0>();
        return it;
    }

    constexpr iterator<true> begin() const
        requires(std::ranges::range<const Vs> && ...) &&
                detail::concatable<const Vs...>
    {
        iterator<true> it(
            this,
            std::in_place_index<0>,
            std::ranges::begin(std::get<0>(M_views)));
        it.template M_satisfy<0>();
        return it;
    }

    constexpr auto end()
        requires(!(detail::simple_view<Vs> && ...))
    {
        if constexpr (detail::last_is_common<Vs...>::value) {
            constexpr auto n = sizeof...(Vs);
            return iterator<false>(
                this,
                std::in_place_index<n - 1>,
                std::ranges::end(std::get<n - 1>(M_views)));
        } else
            return std::default_sentinel;
    }

    constexpr auto end() const
        requires(std::ranges::range<const Vs> && ...) &&
                detail::concatable<const Vs...>
    {
        if constexpr (detail::last_is_common<const Vs...>::value) {
            constexpr auto n = sizeof...(Vs);
            return iterator<true>(
                this,
                std::in_place_index<n - 1>,
                std::ranges::end(std::get<n - 1>(M_views)));
        } else
            return std::default_sentinel;
    }

    constexpr auto size()
        requires(std::ranges::sized_range<Vs> && ...)
    {
        return std::apply(
            [](auto... sizes) {
                using CT = detail::make_unsigned_like_t<
                    std::common_type_t<decltype(sizes)...>>;
                return (CT(sizes) + ...);
            },
            detail::tuple_transform(std::ranges::size, M_views));
    }

    constexpr auto size() const
        requires(std::ranges::sized_range<const Vs> && ...)
    {
        return std::apply(
            [](auto... sizes) {
                using CT = detail::make_unsigned_like_t<
                    std::common_type_t<decltype(sizes)...>>;
                return (CT(sizes) + ...);
            },
            detail::tuple_transform(std::ranges::size, M_views));
    }
};

template <typename... Rs>
concat_view(Rs&&...) -> concat_view<std::views::all_t<Rs>...>;

namespace detail {
template <bool Const, typename... Vs>
struct concat_view_iter_cat {};

template <bool Const, typename... Vs>
    requires detail::all_forward<Const, Vs...>
struct concat_view_iter_cat<Const, Vs...> {
    static auto S_iter_cat()
    {
        if constexpr (!std::is_reference_v<
                          concat_reference_t<maybe_const_t<Const, Vs>...>>)
            return std::input_iterator_tag{};
        else
            return []<typename... Cats>(Cats...) {
                if constexpr (
                    (std::derived_from<Cats, std::random_access_iterator_tag> &&
                     ...) &&
                    concat_is_random_access<Const, Vs...>)
                    return std::random_access_iterator_tag{};
                else if constexpr (
                    (std::derived_from<Cats, std::bidirectional_iterator_tag> &&
                     ...) &&
                    concat_is_bidirectional<Const, Vs...>)
                    return std::bidirectional_iterator_tag{};
                else if constexpr ((std::derived_from<
                                        Cats,
                                        std::forward_iterator_tag> &&
                                    ...))
                    return std::forward_iterator_tag{};
                else
                    return std::input_iterator_tag{};
            }(typename std::iterator_traits<std::ranges::iterator_t<
                       maybe_const_t<Const, Vs>>>::iterator_category{}...);
    }
};
} // namespace detail

template <std::ranges::input_range... Vs>
    requires(std::ranges::view<Vs> && ...) &&
            (sizeof...(Vs) > 0) && detail::concatable<Vs...>
template <bool Const>
class concat_view<Vs...>::iterator
    : public detail::concat_view_iter_cat<Const, Vs...> {
    static auto S_iter_concept()
    {
        if constexpr (detail::concat_is_random_access<Const, Vs...>)
            return std::random_access_iterator_tag{};
        else if constexpr (detail::concat_is_bidirectional<Const, Vs...>)
            return std::bidirectional_iterator_tag{};
        else if constexpr (detail::all_forward<Const, Vs...>)
            return std::forward_iterator_tag{};
        else
            return std::input_iterator_tag{};
    }

    friend concat_view;
    friend iterator<!Const>;

public:
    // iterator_category defined in concat_view_iter_cat
    using iterator_concept = decltype(S_iter_concept());
    using value_type = detail::concat_value_t<maybe_const_t<Const, Vs>...>;
    using difference_type = std::common_type_t<
        std::ranges::range_difference_t<maybe_const_t<Const, Vs>>...>;

private:
    using base_iter =
        std::variant<std::ranges::iterator_t<maybe_const_t<Const, Vs>>...>;

    maybe_const_t<Const, concat_view>* M_parent = nullptr;
    base_iter M_it;

    template <size_t Nm>
    constexpr void M_satisfy()
    {
        if constexpr (Nm < (sizeof...(Vs) - 1)) {
            if (std::get<Nm>(M_it) ==
                std::ranges::end(std::get<Nm>(M_parent->M_views))) {
                M_it.template emplace<Nm + 1>(
                    std::ranges::begin(std::get<Nm + 1>(M_parent->M_views)));
                M_satisfy<Nm + 1>();
            }
        }
    }

    template <size_t Nm>
    constexpr void M_prev()
    {
        if constexpr (Nm == 0)
            --std::get<0>(M_it);
        else {
            if (std::get<Nm>(M_it) ==
                std::ranges::begin(std::get<Nm>(M_parent->M_views))) {
                M_it.template emplace<Nm - 1>(
                    std::ranges::end(std::get<Nm - 1>(M_parent->M_views)));
                M_prev<Nm - 1>();
            } else
                --std::get<Nm>(M_it);
        }
    }

    template <size_t Nm>
    constexpr void M_advance_fwd(difference_type offset, difference_type steps)
    {
        using Dt =
            std::iter_difference_t<std::variant_alternative_t<Nm, base_iter>>;
        if constexpr (Nm == sizeof...(Vs) - 1)
            std::get<Nm>(M_it) += static_cast<Dt>(steps);
        else {
            auto n_size =
                std::ranges::distance(std::get<Nm>(M_parent->M_views));
            if (offset + steps < n_size)
                std::get<Nm>(M_it) += static_cast<Dt>(steps);
            else {
                M_it.template emplace<Nm + 1>(
                    std::ranges::begin(std::get<Nm + 1>(M_parent->M_views)));
                M_advance_fwd<Nm + 1>(0, offset + steps - n_size);
            }
        }
    }

    template <size_t Nm>
    constexpr void M_advance_bwd(difference_type offset, difference_type steps)
    {
        using Dt =
            std::iter_difference_t<std::variant_alternative_t<Nm, base_iter>>;
        if constexpr (Nm == 0)
            std::get<Nm>(M_it) -= static_cast<Dt>(steps);
        else {
            if (offset >= steps)
                std::get<Nm>(M_it) -= static_cast<Dt>(steps);
            else {
                auto prev_size =
                    std::ranges::distance(std::get<Nm - 1>(M_parent->M_views));
                M_it.template emplace<Nm - 1>(
                    std::ranges::end(std::get<Nm - 1>(M_parent->M_views)));
                M_advance_bwd<Nm - 1>(prev_size, steps - offset);
            }
        }
    }

    // Invoke the function object f, which has a call operator with a size_t
    // template parameter (corresponding to an index into the pack of views),
    // using the runtime value of index as the template argument.
    template <typename Fp>
    static constexpr auto S_invoke_with_runtime_index(Fp&& f, size_t index)
    {
        return [&f, index]<size_t Idx>(this auto&& self) {
            if (Idx == index) return f.template operator()<Idx>();
            if constexpr (Idx + 1 < sizeof...(Vs))
                return self.template operator()<Idx + 1>();
        }.template operator()<0>();
    }

    template <typename Fp>
    constexpr auto M_invoke_with_runtime_index(Fp&& f)
    {
        return S_invoke_with_runtime_index(std::forward<Fp>(f), M_it.index());
    }

    template <typename... Args>
    explicit constexpr iterator(
        maybe_const_t<Const, concat_view>* parent,
        Args&&... args)
        requires std::constructible_from<base_iter, Args&&...>
        : M_parent(parent)
        , M_it(std::forward<Args>(args)...)
    {
    }

public:
    iterator() = default;

    constexpr iterator(iterator<!Const> it)
        requires Const && (std::convertible_to<
                               std::ranges::iterator_t<Vs>,
                               std::ranges::iterator_t<const Vs>> &&
                           ...)
        : M_parent(it.M_parent)
    {
        M_invoke_with_runtime_index([this, &it]<size_t Idx>() {
            M_it.template emplace<Idx>(std::get<Idx>(std::move(it.M_it)));
        });
    }

    constexpr decltype(auto) operator*() const
    {
        glibcxx_assert(!M_it.valueless_by_exception());
        using reference =
            detail::concat_reference_t<maybe_const_t<Const, Vs>...>;
        return std::visit([](auto&& it) -> reference { return *it; }, M_it);
    }

    constexpr iterator& operator++()
    {
        M_invoke_with_runtime_index([this]<size_t Idx>() {
            ++std::get<Idx>(M_it);
            M_satisfy<Idx>();
        });
        return *this;
    }

    constexpr void operator++(int) { ++*this; }

    constexpr iterator operator++(int)
        requires detail::all_forward<Const, Vs...>
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr iterator& operator--()
        requires detail::concat_is_bidirectional<Const, Vs...>
    {
        glibcxx_assert(!M_it.valueless_by_exception());
        M_invoke_with_runtime_index([this]<size_t Idx>() { M_prev<Idx>(); });
        return *this;
    }

    constexpr iterator operator--(int)
        requires detail::concat_is_bidirectional<Const, Vs...>
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr iterator& operator+=(difference_type n)
        requires detail::concat_is_random_access<Const, Vs...>
    {
        glibcxx_assert(!M_it.valueless_by_exception());
        M_invoke_with_runtime_index([this, n]<size_t Idx>() {
            auto begin = std::ranges::begin(std::get<Idx>(M_parent->M_views));
            if (n > 0)
                M_advance_fwd<Idx>(std::get<Idx>(M_it) - begin, n);
            else if (n < 0)
                M_advance_bwd<Idx>(std::get<Idx>(M_it) - begin, -n);
        });
        return *this;
    }

    constexpr iterator& operator-=(difference_type n)
        requires detail::concat_is_random_access<Const, Vs...>
    {
        *this += -n;
        return *this;
    }

    constexpr decltype(auto) operator[](difference_type n) const
        requires detail::concat_is_random_access<Const, Vs...>
    {
        return *((*this) + n);
    }

    friend constexpr bool operator==(const iterator& x, const iterator& y)
        requires(
            std::equality_comparable<
                std::ranges::iterator_t<maybe_const_t<Const, Vs>>> &&
            ...)
    {
        glibcxx_assert(!x.M_it.valueless_by_exception());
        glibcxx_assert(!y.M_it.valueless_by_exception());
        return x.M_it == y.M_it;
    }

    friend constexpr bool
    operator==(const iterator& it, std::default_sentinel_t)
    {
        glibcxx_assert(!it.M_it.valueless_by_exception());
        constexpr auto last_idx = sizeof...(Vs) - 1;
        return (
            it.M_it.index() == last_idx &&
            (std::get<last_idx>(it.M_it) ==
             std::ranges::end(std::get<last_idx>(it.M_parent->M_views))));
    }

    friend constexpr bool operator<(const iterator& x, const iterator& y)
        requires detail::all_random_access<Const, Vs...>
    {
        return x.M_it < y.M_it;
    }

    friend constexpr bool operator>(const iterator& x, const iterator& y)
        requires detail::all_random_access<Const, Vs...>
    {
        return x.M_it > y.M_it;
    }

    friend constexpr bool operator<=(const iterator& x, const iterator& y)
        requires detail::all_random_access<Const, Vs...>
    {
        return x.M_it <= y.M_it;
    }

    friend constexpr bool operator>=(const iterator& x, const iterator& y)
        requires detail::all_random_access<Const, Vs...>
    {
        return x.M_it >= y.M_it;
    }

    friend constexpr auto operator<=>(const iterator& x, const iterator& y)
        requires detail::all_random_access<Const, Vs...> &&
                 (std::three_way_comparable<
                      std::ranges::iterator_t<maybe_const_t<Const, Vs>>> &&
                  ...)
    {
        return x.M_it <=> y.M_it;
    }

    friend constexpr iterator operator+(const iterator& it, difference_type n)
        requires detail::concat_is_random_access<Const, Vs...>
    {
        auto r = it;
        return r += n;
    }

    friend constexpr iterator operator+(difference_type n, const iterator& it)
        requires detail::concat_is_random_access<Const, Vs...>
    {
        return it + n;
    }

    friend constexpr iterator operator-(const iterator& it, difference_type n)
        requires detail::concat_is_random_access<Const, Vs...>
    {
        auto r = it;
        return r -= n;
    }

    friend constexpr difference_type
    operator-(const iterator& x, const iterator& y)
        requires detail::concat_is_random_access<Const, Vs...>
    {
        return S_invoke_with_runtime_index(
            [&]<size_t Ix>() -> difference_type {
                return S_invoke_with_runtime_index(
                    [&]<size_t Iy>() -> difference_type {
                        if constexpr (Ix > Iy) {
                            auto dy = std::ranges::distance(
                                std::get<Iy>(y.M_it),
                                std::ranges::end(
                                    std::get<Iy>(y.M_parent->M_views)));
                            auto dx = std::ranges::distance(
                                std::ranges::begin(
                                    std::get<Ix>(x.M_parent->M_views)),
                                std::get<Ix>(x.M_it));
                            difference_type s = 0;
                            [&]<size_t Idx = Iy + 1>(this auto&& self) {
                                if constexpr (Idx < Ix) {
                                    s += std::ranges::size(
                                        std::get<Idx>(x.M_parent->M_views));
                                    self.template operator()<Idx + 1>();
                                }
                            }();
                            return dy + s + dx;
                        } else if constexpr (Ix < Iy)
                            return -(y - x);
                        else
                            return std::get<Ix>(x.M_it) - std::get<Iy>(y.M_it);
                    },
                    y.M_it.index());
            },
            x.M_it.index());
    }

    friend constexpr difference_type
    operator-(const iterator& x, std::default_sentinel_t)
        requires detail::concat_is_random_access<Const, Vs...> &&
                 detail::last_is_common<maybe_const_t<Const, Vs>...>::value
    {
        return S_invoke_with_runtime_index(
            [&]<size_t Ix>() -> difference_type {
                auto dx = std::ranges::distance(
                    std::get<Ix>(x.M_it),
                    std::ranges::end(std::get<Ix>(x.M_parent->M_views)));
                difference_type s = 0;
                [&]<size_t Idx = Ix + 1>(this auto&& self) {
                    if constexpr (Idx < sizeof...(Vs)) {
                        s += std::ranges::size(
                            std::get<Idx>(x.M_parent->M_views));
                        self.template operator()<Idx + 1>();
                    }
                }();
                return -(dx + s);
            },
            x.M_it.index());
    }

    friend constexpr difference_type
    operator-(std::default_sentinel_t, const iterator& x)
        requires detail::concat_is_random_access<Const, Vs...> &&
                 detail::last_is_common<maybe_const_t<Const, Vs>...>::value
    {
        return -(x - std::default_sentinel);
    }

    friend constexpr decltype(auto) iter_move(const iterator& it)
    {
        using Res =
            detail::concat_rvalue_reference_t<maybe_const_t<Const, Vs>...>;
        return std::visit(
            [](const auto& i) -> Res { return std::ranges::iter_move(i); },
            it.M_it);
    }

    friend constexpr void iter_swap(const iterator& x, const iterator& y)
        requires std::swappable_with<
                     std::iter_reference_t<iterator>,
                     std::iter_reference_t<iterator>> &&
                 (... && std::indirectly_swappable<
                             std::ranges::iterator_t<maybe_const_t<Const, Vs>>>)
    {
        std::visit(
            [&]<typename Tp, typename Up>(const Tp& it1, const Up& it2) {
                if constexpr (std::is_same_v<Tp, Up>)
                    std::ranges::iter_swap(it1, it2);
                else
                    std::ranges::swap(*it1, *it2);
            },
            x.M_it,
            y.M_it);
    }
};

namespace detail {
template <typename... Ts>
concept can_concat_view = requires { concat_view(std::declval<Ts>()...); };
} // namespace detail

struct Concat {
    template <typename... Ts>
        requires detail::can_concat_view<Ts...>
    constexpr auto operator() [[nodiscard]] (Ts&&... ts) const
    {
        if constexpr (sizeof...(Ts) == 1)
            return std::views::all(std::forward<Ts>(ts)...);
        else
            return concat_view(std::forward<Ts>(ts)...);
    }
};

inline constexpr Concat concat;

#else

inline constexpr auto concat = std::views::concat;

#endif

} // namespace probfd::views

#endif