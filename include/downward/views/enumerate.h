//
// Created by Thorsten Klößner on 20.04.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef DOWNWARD_VIEWS_ENUMERATE_H
#define DOWNWARD_VIEWS_ENUMERATE_H

#include "downward/views/utils.h"

#include <compare>
#include <concepts>
#include <ranges>
#include <type_traits>

namespace downward::views {

template <class _Rng>
concept Range_with_movable_references =
    std::ranges::input_range<_Rng> &&
    std::move_constructible<std::ranges::range_reference_t<_Rng>> &&
    std::move_constructible<std::ranges::range_rvalue_reference_t<_Rng>>;

template <typename T, typename R>
concept IndexElementPairType = std::constructible_from<
    T,
    std::ranges::range_difference_t<R>,
    std::ranges::range_reference_t<R>>;

template <typename T, typename R>
concept IndexElementPairType2 = std::constructible_from<
    T,
    std::ranges::range_difference_t<R>,
    std::ranges::range_value_t<R>>;

template <typename T, typename R>
concept IndexElementPairType3 = std::constructible_from<
    T,
    std::ranges::range_difference_t<R>,
    std::ranges::range_rvalue_reference_t<R>>;

template <
    std::ranges::view View,
    typename ReferenceType = std::tuple<
        std::ranges::range_difference_t<View>,
        std::ranges::range_reference_t<View>>,
    typename ConstReferenceType = std::tuple<
        std::ranges::range_difference_t<const View>,
        std::ranges::range_reference_t<const View>>,
    typename ValueType = std::tuple<
        std::ranges::range_difference_t<View>,
        std::ranges::range_value_t<View>>,
    typename ConstValueType = std::tuple<
        std::ranges::range_difference_t<const View>,
        std::ranges::range_value_t<const View>>,
    typename RValueReferenceType = std::tuple<
        std::ranges::range_difference_t<View>,
        std::ranges::range_rvalue_reference_t<View>>,
    typename ConstRValueReferenceType = std::tuple<
        std::ranges::range_difference_t<const View>,
        std::ranges::range_rvalue_reference_t<const View>>>
    requires Range_with_movable_references<View> &&
             IndexElementPairType<ReferenceType, View> &&
             IndexElementPairType<ConstReferenceType, const View> &&
             IndexElementPairType2<ValueType, View> &&
             IndexElementPairType2<ConstValueType, const View> &&
             IndexElementPairType3<RValueReferenceType, View> &&
             IndexElementPairType3<ConstRValueReferenceType, const View>
class enumerate_view
    : public std::ranges::view_interface<enumerate_view<View>> {
private:
    /* [[no_unique_address]] */ View range_{};

    template <bool _Const>
    class _Iterator {
    private:
        friend enumerate_view;

        using _Base_t = detail::maybe_const_t<_Const, View>;
        using _Base_iterator = std::ranges::iterator_t<_Base_t>;
        using _Reference_type =
            std::conditional_t<_Const, ReferenceType, ConstReferenceType>;

        /* [[no_unique_address]] */ _Base_iterator _Current{};
        std::ranges::range_difference_t<_Base_t> _Pos = 0;

        constexpr explicit _Iterator(
            _Base_iterator _Current_,
            std::ranges::range_difference_t<_Base_t>
                _Pos_) noexcept(std::
                                    is_nothrow_move_constructible_v<
                                        _Base_iterator>) // strengthened
            : _Current(std::move(_Current_))
            , _Pos(_Pos_)
        {
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::conditional_t<
            std::ranges::random_access_range<_Base_t>,
            std::random_access_iterator_tag,
            std::conditional_t<
                std::ranges::bidirectional_range<_Base_t>,
                std::bidirectional_iterator_tag,
                std::conditional_t<
                    std::ranges::forward_range<_Base_t>,
                    std::forward_iterator_tag,
                    std::input_iterator_tag>>>;
        using difference_type = std::ranges::range_difference_t<_Base_t>;
        using value_type =
            std::conditional_t<_Const, ValueType, ConstValueType>;
        using rval_reference_type = std::conditional_t<
            _Const,
            RValueReferenceType,
            ConstRValueReferenceType>;

        // clang-format off
            _Iterator() requires std::default_initializable<_Base_iterator> = default;
        // clang-format on

        constexpr _Iterator(_Iterator<!_Const> _Other) noexcept(
            std::is_nothrow_constructible_v<
                _Base_iterator,
                std::ranges::iterator_t<View>>) // strengthened
            requires _Const && std::convertible_to<
                                   std::ranges::iterator_t<View>,
                                   _Base_iterator>
            : _Current(std::move(_Other._Current))
            , _Pos(_Other._Pos)
        {
        }

        [[nodiscard]]
        constexpr const _Base_iterator& base() const& noexcept
        {
            return _Current;
        }

        [[nodiscard]]
        constexpr _Base_iterator
        base() && noexcept(std::is_nothrow_move_constructible_v<
                           _Base_iterator>) /* strengthened */
        {
            return std::move(_Current);
        }

        [[nodiscard]]
        constexpr difference_type index() const noexcept
        {
            return _Pos;
        }

        [[nodiscard]]
        constexpr auto operator*() const noexcept(
            noexcept(*_Current) &&
            std::is_nothrow_copy_constructible_v<
                std::ranges::range_reference_t<_Base_t>>) /* strengthened */
        {
            return _Reference_type{_Pos, *_Current};
        }

        constexpr _Iterator&
        operator++() noexcept(noexcept(++_Current)) /* strengthened */
        {
            ++_Current;
            ++_Pos;
            return *this;
        }

        constexpr void
        operator++(int) noexcept(noexcept(++_Current)) /* strengthened */
        {
            ++*this;
        }

        constexpr _Iterator operator++(int) noexcept(
            noexcept(++*this) &&
            std::is_nothrow_copy_constructible_v<_Iterator>) // strengthened
            requires std::ranges::forward_range<_Base_t>
        {
            auto _Tmp = *this;
            ++*this;
            return _Tmp;
        }

        constexpr _Iterator&
        operator--() noexcept(noexcept(--_Current)) // strengthened
            requires std::ranges::bidirectional_range<_Base_t>
        {
            --_Current;
            --_Pos;
            return *this;
        }

        constexpr _Iterator operator--(int) noexcept(
            noexcept(--*this) &&
            std::is_nothrow_copy_constructible_v<_Iterator>) // strengthened
            requires std::ranges::bidirectional_range<_Base_t>
        {
            auto _Tmp = *this;
            --*this;
            return _Tmp;
        }

        constexpr _Iterator& operator+=(const difference_type _Off) noexcept(
            noexcept(_Current += _Off)) // strengthened
            requires std::ranges::random_access_range<_Base_t>
        {
            _Current += _Off;
            _Pos += _Off;
            return *this;
        }

        constexpr _Iterator& operator-=(const difference_type _Off) noexcept(
            noexcept(_Current -= _Off)) // strengthened
            requires std::ranges::random_access_range<_Base_t>
        {
            _Current -= _Off;
            _Pos -= _Off;
            return *this;
        }

        [[nodiscard]]
        constexpr auto operator[](const difference_type _Off) const noexcept(
            noexcept(_Current[_Off]) &&
            std::is_nothrow_copy_constructible_v<
                std::ranges::range_reference_t<_Base_t>>) // strengthened
            requires std::ranges::random_access_range<_Base_t>
        {
            return _Reference_type{
                static_cast<difference_type>(_Pos + _Off),
                _Current[_Off]};
        }

        [[nodiscard]]
        friend constexpr bool
        operator==(const _Iterator& _Left, const _Iterator& _Right) noexcept
        {
            return _Left._Pos == _Right._Pos;
        }

        [[nodiscard]]
        friend constexpr std::strong_ordering
        operator<=>(const _Iterator& _Left, const _Iterator& _Right) noexcept
        {
            return _Left._Pos <=> _Right._Pos;
        }

        [[nodiscard]]
        friend constexpr _Iterator
        operator+(const _Iterator& _It, const difference_type _Off) noexcept(
            std::is_nothrow_copy_constructible_v<_Iterator>  //
            && noexcept(std::declval<_Iterator&>() += _Off)) // strengthened
            requires std::ranges::random_access_range<_Base_t>
        {
            auto _Tmp = _It;
            _Tmp += _Off;
            return _Tmp;
        }

        [[nodiscard]]
        friend constexpr _Iterator
        operator+(const difference_type _Off, const _Iterator& _It) noexcept(
            noexcept(_It + _Off)) // strengthened
            requires std::ranges::random_access_range<_Base_t>
        {
            return _It + _Off;
        }

        [[nodiscard]]
        friend constexpr _Iterator
        operator-(const _Iterator& _It, const difference_type _Off) noexcept(
            std::is_nothrow_copy_constructible_v<_Iterator>  //
            && noexcept(std::declval<_Iterator&>() -= _Off)) // strengthened
            requires std::ranges::random_access_range<_Base_t>
        {
            auto _Tmp = _It;
            _Tmp -= _Off;
            return _Tmp;
        }

        [[nodiscard]]
        friend constexpr difference_type
        operator-(const _Iterator& _Left, const _Iterator& _Right) noexcept
        {
            return _Left._Pos - _Right._Pos;
        }

        [[nodiscard]]
        friend constexpr auto iter_move(const _Iterator& _It) noexcept(
            noexcept(std::ranges::iter_move(_It._Current)) &&
            std::is_nothrow_move_constructible_v<
                std::ranges::range_rvalue_reference_t<_Base_t>>)
        {
            return rval_reference_type{
                _It._Pos,
                std::ranges::iter_move(_It._Current)};
        }
    };

    template <bool _Const>
    class _Sentinel {
    private:
        friend enumerate_view;

        using _Base_t = detail::maybe_const_t<_Const, View>;
        using _Base_sentinel = std::ranges::sentinel_t<_Base_t>;

        /* [[no_unique_address]] */ _Base_sentinel _End{};

        constexpr explicit _Sentinel(_Base_sentinel _End_) noexcept(
            std::is_nothrow_move_constructible_v<
                _Base_sentinel>) // strengthened
            : _End(std::move(_End_))
        {
        }

        template <bool _OtherConst>
            requires std::sentinel_for<
                _Base_sentinel,
                std::ranges::iterator_t<
                    detail::maybe_const_t<_OtherConst, View>>>
        [[nodiscard]]
        constexpr bool _Equal(const _Iterator<_OtherConst>& _It) const
            noexcept(noexcept(_It._Current == _End))
        {
            return _It._Current == _End;
        }

        template <bool _OtherConst>
            requires std::sized_sentinel_for<
                _Base_sentinel,
                std::ranges::iterator_t<
                    detail::maybe_const_t<_OtherConst, View>>>
        [[nodiscard]]
        constexpr std::ranges::range_difference_t<
            detail::maybe_const_t<_OtherConst, View>>
        _Distance_from(const _Iterator<_OtherConst>& _It) const
            noexcept(noexcept(_End - _It._Current))
        {
            return _End - _It._Current;
        }

    public:
        _Sentinel() = default;

        constexpr _Sentinel(_Sentinel<!_Const> _Other) noexcept(
            std::is_nothrow_constructible_v<
                _Base_sentinel,
                std::ranges::sentinel_t<View>>) // strengthened
            requires _Const && std::convertible_to<
                                   std::ranges::sentinel_t<View>,
                                   _Base_sentinel>
            : _End(std::move(_Other._End))
        {
        }

        [[nodiscard]]
        constexpr _Base_sentinel base() const
            noexcept(std::is_nothrow_copy_constructible_v<
                     _Base_sentinel>) /* strengthened
                                       */
        {
            return _End;
        }

        template <bool _OtherConst>
            requires std::sentinel_for<
                _Base_sentinel,
                std::ranges::iterator_t<
                    detail::maybe_const_t<_OtherConst, View>>>
        [[nodiscard]]
        friend constexpr bool operator==(
            const _Iterator<_OtherConst>& _It,
            const _Sentinel&
                _Se) noexcept(noexcept(_Se._Equal(_It))) /* strengthened */
        {
            return _Se._Equal(_It);
        }

        template <bool _OtherConst>
            requires std::sized_sentinel_for<
                _Base_sentinel,
                std::ranges::iterator_t<
                    detail::maybe_const_t<_OtherConst, View>>>
        [[nodiscard]]
        friend constexpr std::ranges::range_difference_t<
            detail::maybe_const_t<_OtherConst, View>>
        operator-(const _Iterator<_OtherConst>& _It, const _Sentinel& _Se) //
            noexcept(noexcept(_Se._Distance_from(_It))) /* strengthened */
        {
            return -_Se._Distance_from(_It);
        }

        template <bool _OtherConst>
            requires std::sized_sentinel_for<
                _Base_sentinel,
                std::ranges::iterator_t<
                    detail::maybe_const_t<_OtherConst, View>>>
        [[nodiscard]]
        friend constexpr std::ranges::range_difference_t<
            detail::maybe_const_t<_OtherConst, View>>
        operator-(const _Sentinel& _Se, const _Iterator<_OtherConst>& _It) //
            noexcept(noexcept(_Se._Distance_from(_It))) /* strengthened */
        {
            return _Se._Distance_from(_It);
        }
    };

    template <class _Rng>
    static constexpr bool _Is_end_nothrow_v =
        std::is_nothrow_move_constructible_v<std::ranges::sentinel_t<_Rng>> //
        && noexcept(std::ranges::end(std::declval<_Rng&>()));

    template <class _Rng>
        requires std::ranges::common_range<_Rng> &&
                     std::ranges::sized_range<_Rng>
    static constexpr bool _Is_end_nothrow_v<_Rng> =
        std::is_nothrow_move_constructible_v<std::ranges::sentinel_t<_Rng>> //
        && noexcept(std::ranges::end(std::declval<_Rng&>()))                //
        && noexcept(std::ranges::distance(std::declval<_Rng&>()));

public:
    // clang-format off
        constexpr enumerate_view() requires std::default_initializable<View> = default;
    // clang-format on

    constexpr explicit enumerate_view(View _Range_) noexcept(
        std::is_nothrow_move_constructible_v<View>) // strengthened
        : range_(std::move(_Range_))
    {
    }

    [[nodiscard]]
    constexpr auto begin() noexcept(
        noexcept(std::ranges::begin(range_)) &&
        std::is_nothrow_move_constructible_v<
            std::ranges::iterator_t<View>>) // strengthened
        requires(!simple_view<View>)
    {
        return _Iterator<false>{std::ranges::begin(range_), 0};
    }

    [[nodiscard]]
    constexpr auto begin() const noexcept(
        noexcept(std::ranges::begin(range_)) &&
        std::is_nothrow_move_constructible_v<
            std::ranges::iterator_t<const View>>) // strengthened
        requires Range_with_movable_references<const View>
    {
        return _Iterator<true>{std::ranges::begin(range_), 0};
    }

    [[nodiscard]]
    constexpr auto end() noexcept(_Is_end_nothrow_v<View>) // strengthened
        requires(!simple_view<View>)
    {
        if constexpr (
            std::ranges::forward_range<View> &&
            std::ranges::common_range<View> && std::ranges::sized_range<View>) {
            return _Iterator<false>{
                std::ranges::end(range_),
                std::ranges::distance(range_)};
        } else {
            return _Sentinel<false>{std::ranges::end(range_)};
        }
    }

    [[nodiscard]]
    constexpr auto end() const
        noexcept(_Is_end_nothrow_v<const View>) // strengthened
        requires Range_with_movable_references<const View>
    {
        if constexpr (
            std::ranges::forward_range<const View> &&
            std::ranges::common_range<const View> &&
            std::ranges::sized_range<const View>) {
            return _Iterator<true>{
                std::ranges::end(range_),
                std::ranges::distance(range_)};
        } else {
            return _Sentinel<true>{std::ranges::end(range_)};
        }
    }

    [[nodiscard]]
    constexpr auto
    size() noexcept(noexcept(std::ranges::size(range_))) // strengthened
        requires std::ranges::sized_range<View>
    {
        return std::ranges::size(range_);
    }

    [[nodiscard]]
    constexpr auto size() const
        noexcept(noexcept(std::ranges::size(range_))) // strengthened
        requires std::ranges::sized_range<const View>
    {
        return std::ranges::size(range_);
    }

    [[nodiscard]]
    constexpr View base() const& noexcept(
        std::is_nothrow_copy_constructible_v<View>) // strengthened
        requires std::copy_constructible<View>
    {
        return range_;
    }

    [[nodiscard]]
    constexpr View base() && noexcept(
        std::is_nothrow_move_constructible_v<View>) /* strengthened */
    {
        return std::move(range_);
    }
};

template <class _Rng>
enumerate_view(_Rng&&) -> enumerate_view<all_t<_Rng>>;

class Enumerate_fn : public std::ranges::range_adaptor_closure<Enumerate_fn> {
public:
    template <std::ranges::viewable_range _Rng>
    [[nodiscard]]
    constexpr auto operator()(_Rng&& _Range) const
        noexcept(noexcept(enumerate_view<all_t<_Rng>>{
            std::forward<_Rng>(_Range)}))
        requires requires {
            enumerate_view<all_t<_Rng>>{std::forward<_Rng>(_Range)};
        }
    {
        return enumerate_view<all_t<_Rng>>{std::forward<_Rng>(_Range)};
    }
};

inline constexpr Enumerate_fn enumerate;

} // namespace downward::views

template <class Rng>
constexpr bool
    std::ranges::enable_borrowed_range<downward::views::enumerate_view<Rng>> =
        std::ranges::enable_borrowed_range<Rng>;

#endif
