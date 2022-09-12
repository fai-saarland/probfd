#ifndef __ITERATORS_H__
#define __ITERATORS_H__

#include <cassert>
#include <functional>
#include <iterator>
#include <utility>
#include <variant>

#include "range_proxy.h"

namespace utils {

struct default_sentinel_t {};

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

    auto& operator*() { return *base; }
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

template <typename IteratorT, typename SentinelT, typename Pred>
class filter_iterator {
public:
    using reference = typename std::iterator_traits<IteratorT>::reference;

    using difference_type =
        typename std::iterator_traits<IteratorT>::difference_type;
    using value_type = typename std::iterator_traits<IteratorT>::value_type;
    using pointer = typename std::iterator_traits<IteratorT>::pointer;
    using iterator_category =
        typename std::iterator_traits<IteratorT>::iterator_category;

    IteratorT base;
    SentinelT end;
    Pred pred;

    filter_iterator(IteratorT b, SentinelT e, Pred predicate)
        : base(std::move(b))
        , end(std::move(e))
        , pred(predicate)
    {
        while (base != end && !pred(*base)) {
            ++base;
        }
    }

    // Prefix increment
    filter_iterator<IteratorT, SentinelT, Pred>& operator++()
    {
        assert(base != end);
        do {
            ++base;
        } while (base != end && !pred(*base));
        return *this;
    }

    // Postfix increment
    filter_iterator<IteratorT, SentinelT, Pred> operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    friend filter_iterator<IteratorT, SentinelT, Pred>
    operator+(filter_iterator<IteratorT, SentinelT, Pred> a, int n)
    {
        std::advance(a, n);
        return a;
    }

    friend filter_iterator<IteratorT, SentinelT, Pred>
    operator-(filter_iterator<IteratorT, SentinelT, Pred> a, int n)
    {
        return a + -n;
    }

    friend difference_type operator-(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const filter_iterator<IteratorT, SentinelT, Pred>& b)
    {
        return std::distance(a, b);
    }

    filter_iterator<IteratorT, SentinelT, Pred>& operator+=(int n)
    {
        std::advance(*this, n);
        return *this;
    }

    filter_iterator<IteratorT, SentinelT, Pred>& operator-=(int n)
    {
        return *this += -n;
    }

    reference operator[](int n) { return *(*this + n); }

    reference operator*() { return *base; }
    pointer operator->() { return base.operator->(); }

    reference operator*() const { return *base; }
    pointer operator->() const { return base.operator->(); }

    friend bool operator==(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const filter_iterator<IteratorT, SentinelT, Pred>& b)
    {
        return a.base == b.base;
    }

    friend bool operator!=(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const filter_iterator<IteratorT, SentinelT, Pred>& b)
    {
        return a.base != b.base;
    }

    friend bool operator<(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const filter_iterator<IteratorT, SentinelT, Pred>& b)
    {
        return a.base < b.base;
    }

    friend bool operator>(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const filter_iterator<IteratorT, SentinelT, Pred>& b)
    {
        return a.base > b.base;
    }

    friend bool operator<=(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const filter_iterator<IteratorT, SentinelT, Pred>& b)
    {
        return a.base <= b.base;
    }

    friend bool operator>=(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const filter_iterator<IteratorT, SentinelT, Pred>& b)
    {
        return a.base >= b.base;
    }

    friend bool operator==(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const default_sentinel_t&)
    {
        return a.base == a.end;
    }

    friend bool operator!=(
        const filter_iterator<IteratorT, SentinelT, Pred>& a,
        const default_sentinel_t&)
    {
        return a.base != a.end;
    }
};

template <template <typename...> typename TupleType, typename... IteratorT>
class zip_iterator {
public:
    using reference =
        TupleType<typename std::iterator_traits<IteratorT>::reference...>;

    using difference_type = std::ptrdiff_t;
    using value_type =
        TupleType<typename std::iterator_traits<IteratorT>::value_type...>;
    using pointer =
        TupleType<typename std::iterator_traits<IteratorT>::pointer...>;
    using iterator_category = std::forward_iterator_tag;

    TupleType<IteratorT...> iterators;

    zip_iterator(IteratorT... iterators)
        : iterators(iterators...)
    {
    }

    static void increment(IteratorT&... t) { (t.operator++(), ...); }

    static void decrement(IteratorT&... t) { (t.operator++(), ...); }

    static decltype(auto) deref(IteratorT&... t)
    {
        return TupleType<
            typename std::iterator_traits<IteratorT>::reference...>(
            t.operator*()...);
    }

    static decltype(auto) point(IteratorT&... t)
    {
        return TupleType<typename std::iterator_traits<IteratorT>::pointer...>(
            t.operator->()...);
    }

    // Prefix increment
    zip_iterator<TupleType, IteratorT...>& operator++()
    {
        std::apply(increment, iterators);
        return *this;
    }

    // Postfix increment
    zip_iterator<TupleType, IteratorT...> operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // Prefix decrement
    zip_iterator<TupleType, IteratorT...>& operator--()
    {
        std::apply(decrement, iterators);
        return *this;
    }

    // Postfix increment
    zip_iterator<TupleType, IteratorT...> operator--(int)
    {
        zip_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    reference operator*() { return std::apply(deref, iterators); }
    pointer operator->() { return std::apply(point, iterators); }

    reference operator*() const { return std::apply(deref, iterators); }
    pointer operator->() const { return std::apply(point, iterators); }

    friend bool operator==(
        const zip_iterator<TupleType, IteratorT...>& a,
        const zip_iterator<TupleType, IteratorT...>& b)
    {
        return a.iterators == b.iterators;
    }

    friend bool operator!=(
        const zip_iterator<TupleType, IteratorT...>& a,
        const zip_iterator<TupleType, IteratorT...>& b)
    {
        return a.iterators != b.iterators;
    }

    friend bool operator<(
        const zip_iterator<TupleType, IteratorT...>& a,
        const zip_iterator<TupleType, IteratorT...>& b)
    {
        return a.iterators < b.iterators;
    }
};

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

    friend bool operator==(
        const variant_iterator<F, T...>& a,
        const variant_iterator<F, T...>& b)
    {
        return a.var == b.var;
    };

    friend bool operator!=(
        const variant_iterator<F, T...>& a,
        const variant_iterator<F, T...>& b)
    {
        return a.var != b.var;
    };
};

template <typename I, typename S>
class common_iterator {
    std::variant<I, S> var;

public:
    using difference_type = typename std::iterator_traits<I>::difference_type;
    using value_type = typename std::iterator_traits<I>::value_type;
    using pointer = typename std::iterator_traits<I>::pointer;
    using reference = typename std::iterator_traits<I>::reference;

    using iterator_category = std::forward_iterator_tag;

    common_iterator() = default;

    common_iterator(I iterator)
        : var(std::move(iterator))
    {
    }

    common_iterator(S iterator)
        : var(std::move(iterator))
    {
    }

    common_iterator<I, S>& operator=(const common_iterator<I, S>&) = default;

    // Prefix increment
    common_iterator<I, S>& operator++()
    {
        std::visit(
            [](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, I>)
                    ++arg;
                else
                    abort();
            },
            var);
        return *this;
    }

    // Postfix increment
    common_iterator<I, S> operator++(int)
    {
        return std::visit(
            [](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, I>)
                    return arg++;
                else
                    abort();
            },
            var);
    }

    reference operator*()
    {
        return std::visit(
            [](auto&& arg) -> reference {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, I>)
                    return *arg;
                else
                    abort();
            },
            var);
    }

    pointer operator->()
    {
        return std::visit(
            [](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, I>)
                    return arg.operator->();
                else
                    abort();
            },
            var);
    }

    reference operator*() const
    {
        return std::visit(
            [](auto&& arg) -> reference {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, I>)
                    return *arg;
                else
                    abort();
            },
            var);
    }

    pointer operator->() const
    {
        return std::visit(
            [](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, I>)
                    return arg.operator->();
                else
                    abort();
            },
            var);
    }

    friend bool
    operator==(const common_iterator<I, S>& a, const common_iterator<I, S>& b)
    {
        return a.var.index() == b.var.index() ||
               std::get<I>(a.var) == std::get<S>(b.var);
    };

    friend bool
    operator!=(const common_iterator<I, S>& a, const common_iterator<I, S>& b)
    {
        return a.var.index() != b.var.index() &&
               std::get<0>(a.var) != std::get<1>(b.var);
    };
};

template <typename IndexSentinelT>
struct slice_sentinel {
    IndexSentinelT indices_end;

    slice_sentinel(IndexSentinelT indices_end)
        : indices_end(indices_end)
    {
    }
};

template <typename IndexIteratorT, typename IteratorT>
class slice_iterator {
public:
    using reference = typename std::iterator_traits<IteratorT>::reference;

    using difference_type = std::ptrdiff_t;
    using value_type = typename std::iterator_traits<IteratorT>::value_type;
    using pointer = typename std::iterator_traits<IteratorT>::pointer;
    using iterator_category = std::random_access_iterator_tag;

    static_assert(std::is_same_v<
                  typename std::iterator_traits<IteratorT>::iterator_category,
                  std::random_access_iterator_tag>);

    IndexIteratorT indices_begin;
    IteratorT base;

    slice_iterator(IndexIteratorT indices_begin, IteratorT base)
        : indices_begin(std::move(indices_begin))
        , base(base)
    {
    }

    // Prefix increment
    slice_iterator<IndexIteratorT, IteratorT>& operator++()
    {
        ++indices_begin;
        return *this;
    }

    // Postfix increment
    slice_iterator<IndexIteratorT, IteratorT> operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // Prefix decrement
    slice_iterator<IndexIteratorT, IteratorT>& operator--()
    {
        --indices_begin;
        return *this;
    }

    // Postfix decrement
    slice_iterator<IndexIteratorT, IteratorT> operator--(int)
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    friend slice_iterator<IndexIteratorT, IteratorT>
    operator+(slice_iterator<IndexIteratorT, IteratorT> a, int n)
    {
        return slice_iterator<IndexIteratorT, IteratorT>(
            a.indices_begin + n,
            a.base);
    }

    friend slice_iterator<IndexIteratorT, IteratorT>
    operator-(slice_iterator<IndexIteratorT, IteratorT> a, int n)
    {
        return slice_iterator<IndexIteratorT, IteratorT>(
            a.indices_begin - n,
            a.base);
    }

    template <typename IndexSentinelT>
    friend difference_type operator-(
        const slice_sentinel<IndexSentinelT>& a,
        const slice_iterator<IndexIteratorT, IteratorT>& b)
    {
        return a.indices_end - b.indices_begin;
    }

    friend difference_type operator-(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_iterator<IndexIteratorT, IteratorT>& b)
    {
        return a.indices_begin - b.indices_begin;
    }

    slice_iterator<IndexIteratorT, IteratorT>& operator+=(int n)
    {
        indices_begin += n;
        return *this;
    }

    slice_iterator<IndexIteratorT, IteratorT>& operator-=(int n)
    {
        indices_begin -= n;
        return *this;
    }

    reference operator[](int n) { return base[indices_begin[n]]; }

    reference operator*() { return base[*indices_begin]; }
    pointer operator->() { return base + *indices_begin; }

    reference operator*() const { return base[*indices_begin]; }
    pointer operator->() const { return base + *indices_begin; }

    friend bool operator==(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_iterator<IndexIteratorT, IteratorT>& b)
    {
        return a.indices_begin == b.indices_begin;
    }

    friend bool operator!=(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_iterator<IndexIteratorT, IteratorT>& b)
    {
        return a.indices_begin != b.indices_begin;
    }

    friend bool operator<(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_iterator<IndexIteratorT, IteratorT>& b)
    {
        return a.indices_begin < b.indices_begin;
    }

    friend bool operator>(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_iterator<IndexIteratorT, IteratorT>& b)
    {
        return a.indices_begin > b.indices_begin;
    }

    friend bool operator<=(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_iterator<IndexIteratorT, IteratorT>& b)
    {
        return a.indices_begin <= b.indices_begin;
    }

    friend bool operator>=(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_iterator<IndexIteratorT, IteratorT>& b)
    {
        return a.indices_begin >= b.indices_begin;
    }

    template <typename IndexSentinelT>
    friend bool operator==(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_sentinel<IndexSentinelT>& b)
    {
        return a.indices_begin == b.indices_end;
    }

    template <typename IndexSentinelT>
    friend bool operator!=(
        const slice_iterator<IndexIteratorT, IteratorT>& a,
        const slice_sentinel<IndexSentinelT>& b)
    {
        return a.indices_begin != b.indices_end;
    }
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

template <typename IteratorT, typename SentinelT, typename Pred>
auto make_filter_iterator(IteratorT base, SentinelT end, Pred pred)
{
    return filter_iterator<IteratorT, SentinelT, Pred>(base, end, pred);
}

template <
    template <typename...> typename TupleType = std::tuple,
    typename... IteratorT>
auto make_zip_iterator(IteratorT... iterators)
{
    return zip_iterator<TupleType, IteratorT...>(iterators...);
}

template <typename... IteratorT>
auto make_common_iterator(IteratorT... iterators)
{
    return common_iterator<IteratorT...>(iterators...);
}

template <typename IndexIteratorT, typename IteratorT>
auto make_slice_iterator(IndexIteratorT index_iterator, IteratorT iterator)
{
    return slice_iterator<IndexIteratorT, IteratorT>(index_iterator, iterator);
}

// Sentinels
inline auto make_filter_sentinel()
{
    return default_sentinel;
}

template <typename SentinelT>
inline auto make_transform_sentinel(SentinelT sentinel)
{
    return transform_sentinel<SentinelT>(sentinel);
}

template <typename IndexSentinelT>
inline auto make_slice_sentinel(IndexSentinelT sentinel)
{
    return slice_sentinel<IndexSentinelT>(sentinel);
}

// Views
template <typename Range, typename Invocable>
auto transform(Range&& range, Invocable f)
{
    return utils::make_range(
        make_transform_iterator(std::begin(range), f),
        make_transform_sentinel(std::end(range)));
}

template <typename Range>
auto keys(Range&& range)
{
    return utils::make_range(
        make_key_iterator(std::begin(range)),
        make_transform_sentinel(std::end(range)));
}

template <typename Range>
auto values(Range&& range)
{
    return utils::make_range(
        make_value_iterator(std::begin(range)),
        make_transform_sentinel(std::end(range)));
}

template <typename Range, typename Pred>
auto filter(Range&& range, Pred pred)
{
    return utils::make_range(
        make_filter_iterator(std::begin(range), std::end(range), pred),
        make_filter_sentinel());
}

template <
    template <typename...> typename TupleType = std::tuple,
    typename... Range>
auto zip(Range&&... ranges)
{
    return utils::make_range(
        make_zip_iterator<TupleType>(ranges.begin()...),
        make_zip_iterator<TupleType>(ranges.end()...));
}

template <typename Range>
auto common(Range&& range)
{
    using IteratorT = decltype(std::begin(range));
    using SentinelT = decltype(std::end(range));
    using common = common_iterator<IteratorT, SentinelT>;

    return utils::make_range(
        common(std::begin(range)),
        common(std::end(range)));
}

template <typename IndexRange, typename Range>
auto slice(IndexRange&& index_range, Range&& range)
{
    using IndexIteratorT = decltype(std::begin(index_range));
    using IndexSentinelT = decltype(std::end(index_range));
    using IteratorT = decltype(std::begin(range));

    return utils::make_range(
        slice_iterator<IndexIteratorT, IteratorT>(
            std::begin(index_range),
            std::begin(range)),
        slice_sentinel<IndexSentinelT>(std::end(index_range)));
}

} // namespace utils

#endif // __ITERATORS_H__
