#ifndef __ITERATORS_H__
#define __ITERATORS_H__

#include <cassert>
#include <compare>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>
#include <variant>

namespace utils {

struct discarding_output_iterator {
    using difference_type = void;
    using value_type = void;
    using pointer = void;
    using reference = void;
    using iterator_category = std::output_iterator_tag;

    /* no-op assignment */
    template <typename T>
    void operator=(T const&)
    {
    }

    discarding_output_iterator& operator++() { return *this; }

    discarding_output_iterator operator++(int) { return *this; }

    discarding_output_iterator& operator*() { return *this; }
};

template <typename Container>
class set_output_iterator {
    std::insert_iterator<Container> base;

public:
    using difference_type = void;
    using value_type = void;
    using pointer = void;
    using reference = void;
    using iterator_category = std::output_iterator_tag;

    set_output_iterator(Container& c)
        : base(c, c.end())
    {
    }

    set_output_iterator& operator++()
    {
        ++base;
        return *this;
    }

    set_output_iterator operator++(int)
    {
        auto& r = *this;
        base++;
        return r;
    }

    auto& operator*() { return *base; }
};

template <typename T>
class infinite_iterator {
    const T value;

public:
    using difference_type = void;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::random_access_iterator_tag;

    template <typename... Args>
    explicit infinite_iterator(Args&&... args)
        : value(std::forward<Args>(args)...)
    {
    }

    // Prefix increment
    infinite_iterator& operator++() { return *this; }

    // Postfix increment
    infinite_iterator operator++(int) { return *this; }

    friend infinite_iterator<T> operator+(const infinite_iterator<T>& a, int)
    {
        return a;
    }

    friend infinite_iterator<T> operator-(const infinite_iterator<T>& a, int)
    {
        return a;
    }

    const T& operator[](int) { return value; }

    reference operator*() const { return value; }
    pointer operator->() const { return &value; }
};

template <typename IteratorT, typename Invocable>
class transform_iterator;

template <typename SentinelT>
struct transform_sentinel {
    using difference_type =
        typename std::iterator_traits<SentinelT>::difference_type;

    SentinelT sentinel;

    transform_sentinel(SentinelT sentinel)
        : sentinel(sentinel)
    {
    }

    template <typename IteratorT, typename Invocable>
    friend difference_type operator-(
        const transform_sentinel<SentinelT>& a,
        const transform_iterator<IteratorT, Invocable>& b)
    {
        return a.sentinel - b.base;
    }
};

template <typename IteratorT, typename Invocable>
class transform_iterator {
public:
    using reference = std::invoke_result_t<
        Invocable,
        typename std::iterator_traits<IteratorT>::reference>;
    using difference_type =
        typename std::iterator_traits<IteratorT>::difference_type;
    using value_type = std::remove_cv_t<std::remove_reference_t<reference>>;
    using pointer = std::add_pointer_t<std::remove_reference_t<reference>>;
    using iterator_category =
        typename std::iterator_traits<IteratorT>::iterator_category;

    IteratorT base;
    Invocable invocable;

    transform_iterator() = default;

    transform_iterator(IteratorT base, Invocable invocable = Invocable())
        : base(std::move(base))
        , invocable(invocable)
    {
    }

    // Prefix increment
    transform_iterator& operator++()
    {
        ++base;
        return *this;
    }

    // Postfix increment
    transform_iterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // Prefix decrement
    transform_iterator& operator--()
    {
        --base;
        return *this;
    }

    // Postfix increment
    transform_iterator operator--(int)
    {
        transform_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    friend transform_iterator<IteratorT, Invocable>
    operator+(const transform_iterator<IteratorT, Invocable>& a, int n)
    {
        return transform_iterator<IteratorT, Invocable>(
            a.base + n,
            a.invocable);
    }

    friend transform_iterator<IteratorT, Invocable>
    operator-(const transform_iterator<IteratorT, Invocable>& a, int n)
    {
        return transform_iterator<IteratorT, Invocable>(
            a.base - n,
            a.invocable);
    }

    friend difference_type operator-(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_iterator<IteratorT, Invocable>& b)
    {
        return a.base - b.base;
    }

    template <typename SentinelT>
    friend difference_type operator-(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_sentinel<SentinelT>& b)
    {
        return a.base - b.sentinel;
    }

    transform_iterator<IteratorT, Invocable>& operator+=(int n)
    {
        base += n;
        return *this;
    }

    transform_iterator<IteratorT, Invocable>& operator-=(int n)
    {
        base -= n;
        return *this;
    }

    reference operator[](int n) { return std::invoke(invocable, base[n]); }

    reference operator*() { return std::invoke(invocable, *base); }
    pointer operator->() { return &std::invoke(invocable, *base); }

    reference operator*() const { return std::invoke(invocable, *base); }
    pointer operator->() const { return &std::invoke(invocable, *base); }

    friend bool operator==(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_iterator<IteratorT, Invocable>& b)
    {
        return a.base == b.base;
    }

    friend bool operator!=(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_iterator<IteratorT, Invocable>& b)
    {
        return a.base != b.base;
    }

    friend bool operator<(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_iterator<IteratorT, Invocable>& b)
    {
        return a.base < b.base;
    }

    friend bool operator>(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_iterator<IteratorT, Invocable>& b)
    {
        return a.base > b.base;
    }

    friend bool operator<=(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_iterator<IteratorT, Invocable>& b)
    {
        return a.base <= b.base;
    }

    friend bool operator>=(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_iterator<IteratorT, Invocable>& b)
    {
        return a.base >= b.base;
    }

    template <typename SentinelT>
    friend bool operator==(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_sentinel<SentinelT>& b)
    {
        return a.base == b.sentinel;
    }

    template <typename SentinelT>
    friend bool operator!=(
        const transform_iterator<IteratorT, Invocable>& a,
        const transform_sentinel<SentinelT>& b)
    {
        return a.base != b.sentinel;
    }
};

template <typename IteratorT>
using key_iterator = transform_iterator<
    IteratorT,
    decltype(&std::iterator_traits<IteratorT>::value_type::first)>;

template <typename IteratorT>
using val_iterator = transform_iterator<
    IteratorT,
    decltype(&std::iterator_traits<IteratorT>::value_type::second)>;

template <typename F, typename... T>
class variant_iterator {
    std::variant<F, T...> var;

    template <typename S>
    using sub_difference_type =
        typename std::iterator_traits<S>::difference_type;

    template <typename S>
    using sub_value_type = typename std::iterator_traits<S>::value_type;

    template <typename S>
    using sub_pointer = typename std::iterator_traits<S>::pointer;

    template <typename S>
    using sub_reference = typename std::iterator_traits<S>::reference;

public:
    using difference_type = typename std::iterator_traits<F>::difference_type;
    using value_type = typename std::iterator_traits<F>::value_type;
    using pointer = typename std::iterator_traits<F>::pointer;
    using reference = typename std::iterator_traits<F>::reference;

    using iterator_category = std::forward_iterator_tag;

    static_assert(
        (std::is_same_v<difference_type, sub_difference_type<T>> && ...));
    static_assert((std::is_same_v<value_type, sub_value_type<T>> && ...));
    static_assert((std::is_same_v<pointer, sub_pointer<T>> && ...));
    static_assert((std::is_same_v<reference, sub_reference<T>> && ...));

    variant_iterator() = default;

    template <typename S>
    variant_iterator(S iterator)
        : var(std::move(iterator))
    {
    }

    // Prefix increment
    variant_iterator<F, T...>& operator++()
    {
        std::visit([](auto&& arg) { ++arg; }, var);
        return *this;
    }

    // Postfix increment
    variant_iterator<F, T...> operator++(int)
    {
        return std::visit(
            [](auto&& arg) { return variant_iterator<F, T...>(arg++); },
            var);
    }

    reference operator*()
    {
        return std::visit([](auto&& arg) -> reference { return *arg; }, var);
    }

    pointer operator->()
    {
        return std::visit([](auto&& arg) { return arg.operator->(); }, var);
    }

    reference operator*() const
    {
        return std::visit([](auto&& arg) -> reference { return *arg; }, var);
    }

    pointer operator->() const
    {
        return std::visit([](auto&& arg) { return arg.operator->(); }, var);
    }

    friend auto operator<=>(
        const variant_iterator<F, T...>& a,
        const variant_iterator<F, T...>& b) = default;
};

// Iterators
template <typename IteratorT, typename Invocable>
auto make_transform_iterator(IteratorT base, Invocable f)
{
    return transform_iterator<IteratorT, Invocable>(base, f);
}

template <typename IteratorT>
auto make_key_iterator(IteratorT base)
{
    using value_type = typename std::iterator_traits<IteratorT>::value_type;
    return make_transform_iterator(base, &value_type::first);
}

template <typename IteratorT>
auto make_val_iterator(IteratorT base)
{
    using value_type = typename std::iterator_traits<IteratorT>::value_type;
    return make_transform_iterator(base, &value_type::second);
}

} // namespace utils

#endif // __ITERATORS_H__
