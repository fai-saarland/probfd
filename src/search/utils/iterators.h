#ifndef __ITERATORS_H__
#define __ITERATORS_H__

#include <iterator>
#include <utility>
#include <variant>

namespace utils {

struct default_sentinel_t {
};

inline constexpr default_sentinel_t default_sentinel{};

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

    set_output_iterator& operator*() { return *base; }
};

template <typename T>
class empty_range_iterator {
public:
    using difference_type = void;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

    // Prefix increment
    empty_range_iterator& operator++() { return *this; }

    // Postfix increment
    empty_range_iterator operator++(int) { return *this; }

    reference operator*() const { abort(); }
    pointer operator->() const { abort(); }

    friend bool
    operator==(const empty_range_iterator&, const empty_range_iterator&)
    {
        return true;
    };

    friend bool
    operator!=(const empty_range_iterator&, const empty_range_iterator&)
    {
        return false;
    };
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
class transform_iterator {
public:
    using reference = std::invoke_result_t<
        Invocable,
        typename std::iterator_traits<IteratorT>::reference>;

    static_assert(
        std::is_reference_v<reference>,
        "Invocable needs to return a reference when invoked on the reference "
        "type of the iterator!");

    using difference_type =
        typename std::iterator_traits<IteratorT>::difference_type;
    using value_type = std::remove_cv_t<std::remove_reference_t<reference>>;
    using pointer = std::add_pointer_t<std::remove_reference_t<reference>>;
    using iterator_category =
        typename std::iterator_traits<IteratorT>::iterator_category;

    IteratorT base;
    Invocable invocable;

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

    reference operator[](int n) { return std::invoke(invocable, base[n]); }

    reference operator*() { return std::invoke(invocable, *base); }
    pointer operator->() { return &std::invoke(invocable, *base); }

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
};

template <typename IteratorT>
using key_iterator = transform_iterator<
    IteratorT,
    decltype(&std::iterator_traits<IteratorT>::value_type::first)>;

template <typename IteratorT>
using val_iterator = transform_iterator<
    IteratorT,
    decltype(&std::iterator_traits<IteratorT>::value_type::second)>;

template <typename IteratorT, typename Invocable>
auto make_transform_iterator(IteratorT base, Invocable f)
{
    return transform_iterator<IteratorT, Invocable>(base, f);
}

template <typename IteratorT>
auto make_key_iterator(IteratorT base)
{
    using ref = typename std::iterator_traits<IteratorT>::reference;
    return make_transform_iterator(base, &std::remove_reference_t<ref>::first);
}

template <typename IteratorT>
auto make_val_iterator(IteratorT base)
{
    using ref = typename std::iterator_traits<IteratorT>::reference;
    return make_transform_iterator(base, &std::remove_reference_t<ref>::second);
}

template <typename T1, typename T2>
struct common_iterator {
    std::variant<T1, T2> var;

    using difference_type1 = typename std::iterator_traits<T1>::difference_type;
    using value_type1 = typename std::iterator_traits<T1>::value_type;
    using pointer1 = typename std::iterator_traits<T1>::pointer;
    using reference1 = typename std::iterator_traits<T1>::reference;

    using difference_type2 = typename std::iterator_traits<T2>::difference_type;
    using value_type2 = typename std::iterator_traits<T2>::value_type;
    using pointer2 = typename std::iterator_traits<T2>::pointer;
    using reference2 = typename std::iterator_traits<T2>::reference;

    static_assert(std::is_same_v<difference_type1, difference_type2>);
    static_assert(std::is_same_v<value_type1, value_type2>);
    static_assert(std::is_same_v<pointer1, pointer2>);
    static_assert(std::is_same_v<reference1, reference2>);

public:
    using difference_type = difference_type1;
    using value_type = value_type1;
    using pointer = pointer1;
    using reference = reference1;
    using iterator_category = std::forward_iterator_tag;

    common_iterator() = default;

    common_iterator(T1 iterator)
        : var(std::move(iterator))
    {
    }

    common_iterator(T2 iterator)
        : var(std::move(iterator))
    {
    }

    // Prefix increment
    common_iterator<T1, T2>& operator++()
    {
        std::visit([](auto&& arg) { ++arg; }, var);
        return *this;
    }

    // Postfix increment
    common_iterator<T1, T2> operator++(int)
    {
        return std::visit(
            [](auto&& arg) { return common_iterator<T1, T2>(arg++); },
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

    friend bool operator==(
        const common_iterator<T1, T2>& a,
        const common_iterator<T1, T2>& b)
    {
        return a.var == b.var;
    };

    friend bool operator!=(
        const common_iterator<T1, T2>& a,
        const common_iterator<T1, T2>& b)
    {
        return a.var != b.var;
    };
};

} // namespace utils

#endif // __ITERATORS_H__
