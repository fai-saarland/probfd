#ifndef DOWNWARD_CONVERT_H
#define DOWNWARD_CONVERT_H

#include "downward/views/utils.h"

#include <concepts>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace downward::views {

template <std::ranges::input_range Vw, typename T>
    requires std::ranges::view<Vw> &&
             std::convertible_to<std::ranges::range_reference_t<Vw>, T>
class convert_view : public std::ranges::view_interface<convert_view<Vw, T>> {
private:
    /* [[no_unique_address]] */
    Vw Range{};

    template <bool Const>
    struct CategoryBase {};

    template <bool Const>
        requires std::ranges::forward_range<detail::maybe_const_t<Const, Vw>>
    struct CategoryBase<Const> {
        using Base = detail::maybe_const_t<Const, Vw>;
        using iterator_category = std::conditional_t<
            std::is_reference_v<T>,
            std::conditional_t<
                std::derived_from<
                    typename std::iterator_traits<
                        std::ranges::iterator_t<Base>>::iterator_category,
                    std::contiguous_iterator_tag>,
                std::random_access_iterator_tag,
                typename std::iterator_traits<
                    std::ranges::iterator_t<Base>>::iterator_category>,
            std::input_iterator_tag>;
    };

    template <bool Const>
    class Iterator : public CategoryBase<Const> {
    private:
        friend convert_view;

        using Parent_t = detail::maybe_const_t<Const, convert_view>;
        using Base = detail::maybe_const_t<Const, Vw>;

        std::ranges::iterator_t<Base> Current{};
        Parent_t* Parent{};

    public:
        using iterator_concept = std::conditional_t<
            std::ranges::random_access_range<Base>,
            std::random_access_iterator_tag,
            std::conditional_t<
                std::ranges::bidirectional_range<Base>,
                std::bidirectional_iterator_tag,
                std::conditional_t<
                    std::ranges::forward_range<Base>,
                    std::forward_iterator_tag,
                    std::input_iterator_tag>>>;

        using value_type = std::remove_cvref_t<T>;
        using difference_type = std::ranges::range_difference_t<Base>;

        Iterator()
            requires std::default_initializable<std::ranges::iterator_t<Base>>
        = default;

        constexpr Iterator(
            Parent_t& Parent_,
            std::ranges::iterator_t<Base>
                Current_) noexcept(std::
                                       is_nothrow_move_constructible_v<
                                           std::ranges::iterator_t<Base>>)
            : Current{std::move(Current_)}
            , Parent{std::addressof(Parent_)}
        {
        }

        constexpr explicit(false) Iterator(Iterator<!Const> It) noexcept(
            std::is_nothrow_constructible_v<
                std::ranges::iterator_t<Base>,
                std::ranges::iterator_t<Vw>>) // strengthened
            requires Const && std::convertible_to<
                                  std::ranges::iterator_t<Vw>,
                                  std::ranges::iterator_t<Base>>
            : Current(std::move(It.Current))
            , Parent(It.Parent)
        {
        }

        [[nodiscard]]
        constexpr const std::ranges::iterator_t<Base>& base() const& noexcept
        {
            return Current;
        }
        [[nodiscard]]
        constexpr std::ranges::iterator_t<Base>
        base() && noexcept(std::is_nothrow_move_constructible_v<
                           std::ranges::iterator_t<Base>>) /* strengthened */
        {
            return std::move(Current);
        }

        [[nodiscard]]
        constexpr decltype(auto) operator*() const
            noexcept(noexcept(static_cast<T>(*Current)))
        {
            return static_cast<T>(*Current);
        }

        constexpr Iterator&
        operator++() noexcept(noexcept(++Current)) /* strengthened */
        {
            ++Current;
            return *this;
        }

        constexpr decltype(auto) operator++(int) noexcept(
            noexcept(++Current) && (!std::ranges::forward_range<Base> ||
                                    std::is_nothrow_copy_constructible_v<
                                        std::ranges::iterator_t<Base>>))
        {
            if constexpr (std::ranges::forward_range<Base>) {
                auto Tmp = *this;
                ++*this;
                return Tmp;
            } else {
                ++*this;
            }
        }

        constexpr Iterator&
        operator--() noexcept(noexcept(--Current)) /* strengthened */
            requires std::ranges::bidirectional_range<Base>
        {
            --Current;
            return *this;
        }

        constexpr Iterator operator--(int) noexcept(
            noexcept(--Current) &&
            std::is_nothrow_copy_constructible_v<std::ranges::iterator_t<Base>>)
            requires std::ranges::bidirectional_range<Base>
        {
            auto Tmp = *this;
            --*this;
            return Tmp;
        }

        constexpr Iterator&
        operator+=(const difference_type Off) noexcept(noexcept(Current += Off))
            requires std::ranges::random_access_range<Base>
        {
            Current += Off;
            return *this;
        }

        constexpr Iterator&
        operator-=(const difference_type Off) noexcept(noexcept(Current -= Off))
            requires std::ranges::random_access_range<Base>
        {
            Current -= Off;
            return *this;
        }

        [[nodiscard]]
        constexpr decltype(auto) operator[](const difference_type Idx) const
            noexcept(noexcept(static_cast<T>(Current[Idx])))
            requires std::ranges::random_access_range<Base>
        {
            return static_cast<T>(Current[Idx]);
        }

        [[nodiscard]]
        friend constexpr bool
        operator==(const Iterator& Left, const Iterator& Right) noexcept(
            noexcept(Left.Current == Right.Current))
            requires std::equality_comparable<std::ranges::iterator_t<Base>>
        {
            return Left.Current == Right.Current;
        }

        [[nodiscard]]
        friend constexpr bool
        operator<(const Iterator& Left, const Iterator& Right) noexcept(
            noexcept(Left.Current < Right.Current)) /* strengthened */
            requires std::ranges::random_access_range<Base>
        {
            return Left.Current < Right.Current;
        }

        [[nodiscard]]
        friend constexpr bool
        operator>(const Iterator& Left, const Iterator& Right) noexcept(
            noexcept(Left.Current < Right.Current)) /* strengthened */
            requires std::ranges::random_access_range<Base>
        {
            return Right < Left;
        }

        [[nodiscard]]
        friend constexpr bool
        operator<=(const Iterator& Left, const Iterator& Right) noexcept(
            noexcept(Left.Current < Right.Current))
            requires std::ranges::random_access_range<Base>
        {
            return !(Right < Left);
        }

        [[nodiscard]]
        friend constexpr bool
        operator>=(const Iterator& Left, const Iterator& Right) noexcept(
            noexcept(Left.Current < Right.Current)) /* strengthened */
            requires std::ranges::random_access_range<Base>
        {
            return !(Left < Right);
        }

        [[nodiscard]]
        friend constexpr auto
        operator<=>(const Iterator& Left, const Iterator& Right) noexcept(
            noexcept(Left.Current <=> Right.Current))
            requires std::ranges::random_access_range<Base> &&
                     std::three_way_comparable<std::ranges::iterator_t<Base>>
        {
            return Left.Current <=> Right.Current;
        }

        [[nodiscard]]
        friend constexpr Iterator
        operator+(Iterator It, const difference_type Off) noexcept(
            noexcept(It.Current += Off)) /* strengthened */
            requires std::ranges::random_access_range<Base>
        {
            It.Current += Off;
            return It;
        }

        [[nodiscard]]
        friend constexpr Iterator
        operator+(const difference_type Off, Iterator It) noexcept(
            noexcept(It.Current += Off)) /* strengthened */
            requires std::ranges::random_access_range<Base>
        {
            It.Current += Off;
            return It;
        }

        [[nodiscard]]
        friend constexpr Iterator
        operator-(Iterator It, const difference_type Off) noexcept(
            noexcept(It.Current -= Off)) /* strengthened */
            requires std::ranges::random_access_range<Base>
        {
            It.Current -= Off;
            return It;
        }

        [[nodiscard]]
        friend constexpr difference_type
        operator-(const Iterator& Left, const Iterator& Right) noexcept(
            noexcept(Left.Current - Right.Current))
            requires std::sized_sentinel_for<
                std::ranges::iterator_t<Base>,
                std::ranges::iterator_t<Base>>
        {
            return Left.Current - Right.Current;
        }
    };

    template <bool Const>
    class Sentinel {
    private:
        friend convert_view;

        using Parent_t = detail::maybe_const_t<Const, convert_view>;
        using Base = detail::maybe_const_t<Const, Vw>;

        template <bool OtherConst>
        using const_iter =
            std::ranges::iterator_t<detail::maybe_const_t<OtherConst, Vw>>;

        std::ranges::sentinel_t<Base> Last{};

        template <bool OtherConst>
        [[nodiscard]]
        static constexpr const const_iter<OtherConst>&
        GetCurrent(const Iterator<OtherConst>& It) noexcept
        {
            return It.Current;
        }

    public:
        Sentinel() = default;

        constexpr explicit Sentinel(
            std::ranges::sentinel_t<Base>
                Last_) noexcept(std::
                                    is_nothrow_move_constructible_v<
                                        std::ranges::sentinel_t<Base>>)
            : Last(std::move(Last_))
        {
        }

        constexpr explicit(false) Sentinel(Sentinel<!Const> Se) noexcept(
            std::is_nothrow_constructible_v<
                std::ranges::sentinel_t<Base>,
                std::ranges::sentinel_t<Vw>>)
            requires Const && std::convertible_to<
                                  std::ranges::sentinel_t<Vw>,
                                  std::ranges::sentinel_t<Base>>
            : Last(std::move(Se.Last))
        {
        }

        [[nodiscard]]
        constexpr std::ranges::sentinel_t<Base> base() const noexcept(
            std::is_nothrow_copy_constructible_v<std::ranges::sentinel_t<Base>>)
        {
            return Last;
        }

        template <bool OtherConst>
            requires std::sentinel_for<
                std::ranges::sentinel_t<Base>,
                const_iter<OtherConst>>
        [[nodiscard]]
        friend constexpr bool operator==(
            const Iterator<OtherConst>& Left,
            const Sentinel&
                Right) noexcept(noexcept(GetCurrent(Left) == Right.Last))
        {
            return GetCurrent(Left) == Right.Last;
        }

        template <bool OtherConst>
            requires std::sized_sentinel_for<
                std::ranges::sentinel_t<Base>,
                const_iter<OtherConst>>
        [[nodiscard]]
        friend constexpr std::ranges::range_difference_t<
            detail::maybe_const_t<OtherConst, Vw>>
        operator-(
            const Iterator<OtherConst>& Left,
            const Sentinel&
                Right) noexcept(noexcept(GetCurrent(Left) - Right.Last))
        {
            return GetCurrent(Left) - Right.Last;
        }

        template <bool OtherConst>
            requires std::sized_sentinel_for<
                std::ranges::sentinel_t<Base>,
                const_iter<OtherConst>>
        [[nodiscard]]
        friend constexpr std::ranges::range_difference_t<
            detail::maybe_const_t<OtherConst, Vw>>
        operator-(
            const Sentinel& Left,
            const Iterator<OtherConst>&
                Right) noexcept(noexcept(Left.Last - GetCurrent(Right)))
        {
            return Left.Last - GetCurrent(Right);
        }
    };

public:
    convert_view()
        requires std::default_initializable<Vw>
    = default;

    constexpr explicit convert_view(Vw Range_) noexcept(
        std::is_nothrow_move_constructible_v<Vw>)
        : Range(std::move(Range_))
    {
    }

    [[nodiscard]]
    constexpr Vw
    base() const& noexcept(std::is_nothrow_copy_constructible_v<Vw>)
        requires std::copy_constructible<Vw>
    {
        return Range;
    }

    [[nodiscard]]
    constexpr Vw base() && noexcept(std::is_nothrow_move_constructible_v<Vw>)
    {
        return std::move(Range);
    }

    [[nodiscard]]
    constexpr Iterator<false> begin() noexcept(
        noexcept(std::ranges::begin(Range)) &&
        std::is_nothrow_move_constructible_v<std::ranges::iterator_t<Vw>>)
    {
        return Iterator<false>(*this, std::ranges::begin(Range));
    }

    [[nodiscard]]
    constexpr Iterator<true> begin() const noexcept(
        noexcept(std::ranges::begin(Range)) &&
        std::is_nothrow_move_constructible_v<std::ranges::iterator_t<Vw>>)
        requires std::ranges::range<const Vw>
    {
        return Iterator<true>(*this, std::ranges::begin(Range));
    }

    [[nodiscard]]
    constexpr auto end() noexcept(
        noexcept(std::ranges::end(Range)) &&
        std::is_nothrow_move_constructible_v<decltype(std::ranges::end(Range))>)
    {
        if constexpr (std::ranges::common_range<Vw>) {
            return Iterator<false>(*this, std::ranges::end(Range));
        } else {
            return Sentinel<false>(std::ranges::end(Range));
        }
    }

    [[nodiscard]]
    constexpr auto end() const noexcept(
        noexcept(std::ranges::end(Range)) &&
        std::is_nothrow_move_constructible_v<decltype(std::ranges::end(Range))>)
        requires std::ranges::range<const Vw>
    {
        if constexpr (std::ranges::common_range<Vw>) {
            return Iterator<true>(*this, std::ranges::end(Range));
        } else {
            return Sentinel<true>(std::ranges::end(Range));
        }
    }

    [[nodiscard]]
    constexpr auto size() noexcept(noexcept(std::ranges::size(Range)))
        requires std::ranges::sized_range<Vw>
    {
        return std::ranges::size(Range);
    }

    [[nodiscard]]
    constexpr auto size() const noexcept(noexcept(std::ranges::size(Range)))
        requires std::ranges::sized_range<const Vw>
    {
        return std::ranges::size(Range);
    }
};

namespace detail {

template <typename T>
using all_t = decltype(std::views::all(std::declval<T>()));

template <typename T>
struct convert_fn : public std::ranges::range_adaptor_closure<convert_fn<T>> {
    template <std::ranges::viewable_range Rng>
    [[nodiscard]]
    constexpr auto operator()(Rng&& Range) const noexcept(
        noexcept(convert_view<all_t<Rng>, T>(std::forward<Rng>(Range))))
        requires requires {
            convert_view<all_t<Rng>, T>(static_cast<Rng&&>(Range));
        }
    {
        return convert_view<all_t<Rng>, T>(std::forward<Rng>(Range));
    }
};

} // namespace detail

template <typename T>
inline constexpr detail::convert_fn<T> convert;

} // namespace downward::views

#endif
