#ifndef PROBFD_DYNAMIC_CARTESIAN_PRODUCT_H
#define PROBFD_DYNAMIC_CARTESIAN_PRODUCT_H

#include <algorithm>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <vector>

#include "probfd/views/fold.h"

namespace probfd::views {

template <bool Const, typename T>
using maybe_const_t = std::conditional_t<Const, const T, T>;

template <std::ranges::viewable_range Range>
using all_t = decltype(std::views::all(std::declval<Range>()));

template <typename Range>
concept dsimple_view =
    std::ranges::view<Range> && std::ranges::range<const Range> &&
    std::same_as<
        std::ranges::iterator_t<Range>,
        std::ranges::iterator_t<const Range>> &&
    std::same_as<
        std::ranges::sentinel_t<Range>,
        std::ranges::sentinel_t<const Range>>;

namespace detail {

template <bool Const, typename R>
concept dcartesian_product_is_random_access =
    std::ranges::random_access_range<maybe_const_t<Const, R>> &&
    std::ranges::sized_range<maybe_const_t<Const, R>>;

template <typename Range>
concept dcartesian_product_common_arg =
    std::ranges::common_range<Range> ||
    (std::ranges::sized_range<Range> &&
     std::ranges::random_access_range<Range>);

template <bool Const, typename R>
concept dcartesian_product_is_bidirectional =
    std::ranges::bidirectional_range<maybe_const_t<Const, R>>;

template <bool Const, template <typename> class RSent, typename R>
concept dcartesian_is_sized_sentinel = std::sized_sentinel_for<
    RSent<maybe_const_t<Const, R>>,
    std::ranges::iterator_t<maybe_const_t<Const, R>>>;

template <dcartesian_product_common_arg Range>
constexpr auto dcartesian_common_arg_end(Range&& r)
{
    if constexpr (std::ranges::common_range<Range>)
        return std::ranges::end(r);
    else
        return std::ranges::begin(r) + std::ranges::distance(r);
}
} // namespace detail

template <typename R, typename T>
concept container_compatible_range =
    std::ranges::input_range<R> &&
    std::convertible_to<std::ranges::range_reference_t<R>, T>;

template <typename T>
    requires std::is_object_v<T>
class array;

template <typename T>
bool operator==(const array<T>& left, const array<T>& right);

template <typename T>
bool operator<=>(const array<T>& left, const array<T>& right);

template <typename T>
    requires std::is_object_v<T>
class array {
    std::vector<T> data;

    template <bool Const>
    class Iterator;

public:
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    template <container_compatible_range<T> R>
    array(std::from_range_t, R&& rg)
        : data(std::from_range, std::forward<R>(rg))
    {
    }

    template <typename U>
    explicit(false) array(const array<U>& other)
        requires(
            std::convertible_to<std::ranges::range_reference_t<array<U>>, T>)
        : data(std::from_range, other)
    {
    }

    array(const array<T>& other) = default;
    array(array<T>&& other) = default;

    array<T>& operator=(const array<T>& other) = default;
    array<T>& operator=(array<T>&& other) = default;

    auto begin() { return Iterator<false>(data.begin()); }
    auto begin() const { return Iterator<true>(data.begin()); }

    auto end() { return Iterator<false>(data.end()); }
    auto end() const { return Iterator<true>(data.end()); }

    auto size() const { return data.size(); }

    decltype(auto) operator[](typename decltype(data)::size_type i)
    {
        return data[i];
    }

    decltype(auto) operator[](decltype(data)::size_type i) const
    {
        return data[i];
    }

    friend bool operator== <>(const array<T>& left, const array<T>& right);
    friend bool operator<=> <>(const array<T>& left, const array<T>& right);
};

template <typename T>
inline bool operator==(const array<T>& left, const array<T>& right)
{
    return left.data == right.data;
}

template <typename T>
inline bool operator<=>(const array<T>& left, const array<T>& right)
{
    return left.data <=> right.data;
}

template <typename T>
    requires std::is_object_v<T>
template <bool Const>
class array<T>::Iterator {
    friend class array<T>;

    using IType = std::conditional_t<
        Const,
        typename std::vector<T>::const_iterator,
        typename std::vector<T>::iterator>;

    IType underlying;

    explicit Iterator(IType underlying)
        : underlying(underlying)
    {
    }

public:
    using iterator_category =
        typename std::iterator_traits<IType>::iterator_category;

    using difference_type =
        typename std::iterator_traits<IType>::difference_type;

    using iterator_concept = typename IType::iterator_concept;

    using value_type = std::iter_value_t<IType>;

    Iterator() = default;

    constexpr explicit(false) Iterator(const Iterator<Const>& i)
        : underlying(i.underlying)
    {
    }

    constexpr explicit(false) Iterator(Iterator<!Const> i)
        requires Const
        : underlying(i.underlying)
    {
    }

    constexpr decltype(auto) operator*() const { return *underlying; }

    constexpr decltype(auto) operator->() const
    {
        return underlying.operator->();
    }

    constexpr Iterator& operator++()
    {
        ++underlying;
        return *this;
    }

    constexpr Iterator operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr Iterator& operator--()
    {
        --underlying;
        return *this;
    }

    constexpr Iterator operator--(int)
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr Iterator& operator+=(difference_type x)
    {
        underlying += x;
        return *this;
    }

    constexpr Iterator& operator-=(difference_type x) { return *this += -x; }

    constexpr decltype(auto) operator[](difference_type n) const
    {
        return *((*this) + n);
    }

    friend constexpr bool operator==(const Iterator& x, const Iterator& y)
    {
        return x.underlying == y.underlying;
    }

    friend constexpr auto operator<=>(const Iterator& x, const Iterator& y)
    {
        return x.underlying <=> y.underlying;
    }

    friend constexpr Iterator operator+(Iterator x, difference_type y)
    {
        return x += y;
    }

    friend constexpr Iterator operator+(difference_type x, Iterator y)
    {
        return y += x;
    }

    friend constexpr Iterator operator-(Iterator x, difference_type y)
    {
        return x -= y;
    }

    friend constexpr difference_type
    operator-(const Iterator& x, const Iterator& y)
    {
        return x.underlying - y.underlying;
    }

    friend constexpr void iter_swap(const Iterator& l, const Iterator& r)
    {
        using std::iter_swap;
        iter_swap(l.underlying, r.underlying);
    }

    friend constexpr decltype(auto) iter_move(const Iterator& l)
    {
        using std::ranges::iter_move;
        return iter_move(l.underlying);
    }
};

template <std::ranges::input_range R>
array(std::from_range_t, R&&) -> array<std::ranges::range_reference_t<R>>;

template <std::ranges::bidirectional_range OR>
    requires std::ranges::view<OR> &&
             std::ranges::input_range<std::ranges::range_reference_t<OR>> &&
             std::ranges::borrowed_range<std::ranges::range_reference_t<OR>> &&
             std::ranges::viewable_range<std::ranges::range_reference_t<OR>>
class dynamic_cartesian_product_view
    : public std::ranges::view_interface<dynamic_cartesian_product_view<OR>> {
    using IR = std::views::all_t<std::ranges::range_reference_t<OR>>;
    using IRIT = std::ranges::iterator_t<IR>;

    using CIR = std::views::all_t<std::ranges::range_reference_t<const OR>>;
    using CIRIT = std::ranges::iterator_t<CIR>;

    template <bool>
    class Iterator;

    using S_difference_type =
        std::common_type_t<ptrdiff_t, std::ranges::range_difference_t<IR>>;

    using ST = std::make_unsigned_t<S_difference_type>;

    OR M_bases;
    std::vector<ST> M_multipliers;
    ST size_;

public:
    dynamic_cartesian_product_view() = default;

    constexpr explicit dynamic_cartesian_product_view(OR r)
        : M_bases(std::move(r))
        , M_multipliers(std::ranges::size(r))
    {
        ST multiplier = 1;

        auto rit = M_bases.end();
        auto rend = M_bases.begin();
        auto it = M_multipliers.end();

        while (rit != rend) {
            --rit;
            --it;
            *it = multiplier;
            multiplier *=
                static_cast<ST>(std::ranges::size(std::views::all(*rit)));
        }

        size_ = multiplier;
    }

    constexpr Iterator<false> begin()
        requires(!dsimple_view<OR>)
    {
        return Iterator<false>(
            *this,
            std::vector<IRIT>(
                std::from_range,
                M_bases | std::views::transform(std::views::all) |
                    std::views::transform(std::ranges::begin)));
    }

    constexpr Iterator<true> begin() const
    {
        return Iterator<true>(
            *this,
            std::vector<CIRIT>(
                std::from_range,
                M_bases | std::views::transform(std::views::all) |
                    std::views::transform(std::ranges::begin)));
    }

    constexpr Iterator<false> end()
        requires(!dsimple_view<OR> && detail::dcartesian_product_common_arg<IR>)
    {
        std::vector<IRIT> ends(
            std::from_range,
            M_bases | std::views::transform(std::views::all) |
                std::views::transform(std::ranges::begin));

        if (std::ranges::none_of(M_bases, std::ranges::empty)) {
            ends.front() = detail::dcartesian_common_arg_end(
                *std::ranges::begin(M_bases) | std::views::all);
        }

        return Iterator<false>(*this, std::move(ends));
    }

    constexpr Iterator<true> end() const
        requires detail::dcartesian_product_common_arg<const IR>
    {
        std::vector<CIRIT> ends(
            std::from_range,
            M_bases | std::views::transform(std::views::all) |
                std::views::transform(std::ranges::begin));

        if (std::ranges::none_of(M_bases, std::ranges::empty)) {
            ends.front() = detail::dcartesian_common_arg_end(
                *std::ranges::begin(M_bases) | std::views::all);
        }

        return Iterator<true>(*this, std::move(ends));
    }

    constexpr std::default_sentinel_t end() const noexcept
    {
        return std::default_sentinel;
    }

    constexpr auto size()
        requires std::ranges::sized_range<IR>
    {
        return size_;
    }

    constexpr auto size() const
        requires std::ranges::sized_range<const IR>
    {
        return size_;
    }
};

template <typename R>
dynamic_cartesian_product_view(R&&)
    -> dynamic_cartesian_product_view<std::views::all_t<R>>;

template <std::ranges::bidirectional_range OR>
    requires std::ranges::view<OR> &&
             std::ranges::input_range<std::ranges::range_reference_t<OR>> &&
             std::ranges::borrowed_range<std::ranges::range_reference_t<OR>> &&
             std::ranges::viewable_range<std::ranges::range_reference_t<OR>>
template <bool Const>
class dynamic_cartesian_product_view<OR>::Iterator {
    using IR = std::views::all_t<
        std::ranges::range_reference_t<maybe_const_t<Const, OR>>>;
    using Parent = maybe_const_t<Const, dynamic_cartesian_product_view>;

    Parent* M_parent = nullptr;

    std::vector<std::ranges::iterator_t<IR>> M_current;

    constexpr Iterator(Parent& parent, decltype(M_current) current)
        : M_parent(std::addressof(parent))
        , M_current(std::move(current))
    {
    }

    static auto S_iter_concept()
    {
        if constexpr (detail::dcartesian_product_is_random_access<Const, IR>)
            return std::random_access_iterator_tag{};
        else if constexpr (detail::
                               dcartesian_product_is_bidirectional<Const, IR>)
            return std::bidirectional_iterator_tag{};
        else if constexpr (std::ranges::forward_range<maybe_const_t<Const, IR>>)
            return std::forward_iterator_tag{};
        else
            return std::input_iterator_tag{};
    }

    friend dynamic_cartesian_product_view;

public:
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = decltype(S_iter_concept());
    using value_type = array<std::ranges::range_value_t<IR>>;
    using difference_type = dynamic_cartesian_product_view::S_difference_type;

    Iterator() = default;

    constexpr explicit(false) Iterator(Iterator<!Const> i)
        requires Const && std::convertible_to<
                              std::ranges::iterator_t<IR>,
                              std::ranges::iterator_t<const IR>>
        : M_parent(i.M_parent)
        , M_current(std::from_range, i.M_current)
    {
    }

    constexpr auto operator*() const
    {
        auto f = [](std::ranges::iterator_t<IR> i) -> auto {
            if constexpr (std::is_lvalue_reference_v<decltype(*i)>)
                return std::ref(*i);
            else
                return *i;
        };

        return M_current | std::views::transform(f) | std::ranges::to<array>();
    }

    constexpr Iterator& operator++()
    {
        auto base_it = std::prev(M_parent->M_bases.end());
        auto it = std::prev(M_current.end());

        for (; base_it != M_parent->M_bases.begin(); --base_it, --it) {
            if (++*it != std::ranges::end(*base_it)) return *this;
            *it = std::ranges::begin(*base_it);
        }

        ++*it;
        return *this;
    }

    constexpr void operator++(int) { ++*this; }

    constexpr Iterator operator++(int)
        requires std::ranges::forward_range<maybe_const_t<Const, IR>>
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr Iterator& operator--()
        requires detail::dcartesian_product_is_bidirectional<Const, IR>
    {
        auto base_it = std::prev(M_parent->M_bases.end());
        auto it = std::prev(M_current.end());

        for (; base_it != M_parent->M_bases.begin(); --base_it, --it) {
            if (*it != std::ranges::begin(*base_it)) {
                --*it;
                return *this;
            }
            *it = detail::dcartesian_common_arg_end(*base_it);
        }

        --*it;
        return *this;
    }

    constexpr Iterator operator--(int)
        requires detail::dcartesian_product_is_bidirectional<Const, IR>
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr Iterator& operator+=(difference_type x)
        requires detail::dcartesian_product_is_random_access<Const, IR>
    {
        auto base_it = std::prev(M_parent->M_bases.end());
        auto it = std::prev(M_current.end());

        for (; base_it != M_parent->M_bases.begin(); --base_it, --it) {
            if (x == 0) return *this;

            auto size = std::ranges::ssize(*base_it);
            auto begin = std::ranges::begin(*base_it);

            auto offset = *it - begin;
            offset += x;
            x = offset / size;
            offset %= size;

            if (offset < 0) {
                offset = size + offset;
                --x;
            }

            *it = begin + offset;
        }

        *it += x;

        return *this;
    }

    constexpr Iterator& operator-=(difference_type x)
        requires detail::dcartesian_product_is_random_access<Const, IR>
    {
        return *this += -x;
    }

    constexpr decltype(auto) operator[](difference_type n) const
        requires detail::dcartesian_product_is_random_access<Const, IR>
    {
        return *((*this) + n);
    }

    friend constexpr bool operator==(const Iterator& x, const Iterator& y)
        requires std::equality_comparable<
            std::ranges::iterator_t<maybe_const_t<Const, IR>>>
    {
        return x.M_current == y.M_current;
    }

    friend constexpr bool operator==(const Iterator& x, std::default_sentinel_t)
    {
        return std::ranges::any_of(
            std::views::zip(x.M_current, x.M_parent->M_bases),
            std::equal_to<decltype(x.M_current)>{});
    }

    friend constexpr auto operator<=>(const Iterator& x, const Iterator& y)
        requires std::ranges::random_access_range<maybe_const_t<Const, IR>>
    {
        return x.M_current <=> y.M_current;
    }

    friend constexpr Iterator operator+(Iterator x, difference_type y)
        requires detail::dcartesian_product_is_random_access<Const, IR>
    {
        return x += y;
    }

    friend constexpr Iterator operator+(difference_type x, Iterator y)
        requires detail::dcartesian_product_is_random_access<Const, IR>
    {
        return y += x;
    }

    friend constexpr Iterator operator-(Iterator x, difference_type y)
        requires detail::dcartesian_product_is_random_access<Const, IR>
    {
        return x -= y;
    }

    friend constexpr difference_type
    operator-(const Iterator& x, const Iterator& y)
        requires detail::
            dcartesian_is_sized_sentinel<Const, std::ranges::iterator_t, IR>
    {
        auto mit = x.get_multipliers().begin();
        auto mend = std::prev(x.get_multipliers().end());

        auto xit = x.M_current.begin();
        auto yit = y.M_current.begin();

        difference_type dist = 0;

        for (; mit != mend; ++mit, ++xit, ++yit) {
            dist += *mit * static_cast<difference_type>(*xit - *yit);
        }

        return dist;
    }

    friend constexpr difference_type
    operator-(const Iterator& x, std::default_sentinel_t)
        requires detail::
            dcartesian_is_sized_sentinel<Const, std::ranges::sentinel_t, IR>
    {
        auto mit = x.get_multipliers().begin();
        auto mend = std::prev(x.get_multipliers().end());

        auto xit = x.M_current.begin();
        auto rit = x.M_parent->M_bases.begin();

        difference_type dist = 0;

        for (; mit != mend; ++mit, ++xit, ++rit) {
            dist += *mit *
                    static_cast<difference_type>(*xit - std::ranges::end(*rit));
        }

        return dist;
    }

    friend constexpr difference_type
    operator-(std::default_sentinel_t, const Iterator& i)
        requires detail::
            dcartesian_is_sized_sentinel<Const, std::ranges::sentinel_t, IR>
    {
        return -(i - std::default_sentinel);
    }

    friend constexpr void iter_swap(const Iterator& l, const Iterator& r)
        requires(std::indirectly_swappable<
                 std::ranges::iterator_t<maybe_const_t<Const, IR>>>)
    {
        for (auto& [lit, rit] : std::views::zip(l.M_current, r.M_current)) {
            std::ranges::iter_swap(lit, rit);
        }
    }

private:
    const auto& get_multipliers() const { return M_parent->M_multipliers; }
};

namespace detail {

template <typename T>
using all_t = decltype(std::views::all(std::declval<T>()));

template <typename T>
concept can_dynamic_cartesian_product_view =
    std::ranges::view<all_t<T>> &&
    std::ranges::input_range<std::ranges::range_reference_t<all_t<T>>> &&
    std::ranges::borrowed_range<std::ranges::range_reference_t<all_t<T>>> &&
    std::ranges::viewable_range<std::ranges::range_reference_t<all_t<T>>>;

struct DynamicCartesianProduct
    : public std::ranges::range_adaptor_closure<DynamicCartesianProduct> {
    template <typename T>
        requires(detail::can_dynamic_cartesian_product_view<T>)
    constexpr auto operator() [[nodiscard]] (T&& t) const
    {
        return dynamic_cartesian_product_view<all_t<T>>(std::forward<T>(t));
    }
};

template <typename T>
struct wrap_ref {
    using type = T;
};

template <typename T>
    requires std::is_reference_v<T>
struct wrap_ref<T> {
    using type = std::reference_wrapper<std::remove_reference_t<T>>;
};

template <typename T>
using wrap_ref_t = typename wrap_ref<T>::type;

} // namespace detail

inline constexpr detail::DynamicCartesianProduct dynamic_cartesian_product;

} // namespace probfd::views

template <typename T>
constexpr bool std::ranges::enable_borrowed_range<probfd::views::array<T>> =
    true;

template <
    class T,
    class U,
    template <class>
    class TQual,
    template <class>
    class UQual>
struct std::basic_common_reference<
    probfd::views::array<T>,
    probfd::views::array<U>,
    TQual,
    UQual> {
    using type = probfd::views::array<probfd::views::detail::wrap_ref_t<
        std::common_reference_t<TQual<T>, UQual<U>>>>;
};

#endif // PROBFD_DYNAMIC_CARTESIAN_PRODUCT_H
