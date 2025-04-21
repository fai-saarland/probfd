#ifndef DOWNWARD_PROXY_COLLECTION_H
#define DOWNWARD_PROXY_COLLECTION_H

#include <cassert>
#include <compare>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <ranges>

namespace downward {

template <typename T>
concept SizedSubscriptable = requires(const T c, std::size_t s) {
    c.operator[](s);
    { c.size() } -> std::convertible_to<std::size_t>;
};

template <typename T>
class ProxyCollection : public std::ranges::view_interface<ProxyCollection<T>> {
    class ProxyIterator {
        /* We store a pointer to collection instead of a reference
           because iterators have to be copy assignable. */
        const T* collection;
        std::size_t pos;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type =
            std::remove_cvref_t<decltype(std::declval<T>().operator[](0U))>;
        using difference_type = int;
        using pointer = const value_type*;
        using reference = value_type;

        ProxyIterator() = default;

        ProxyIterator(const T& collection, std::size_t pos)
            : collection(&collection)
            , pos(pos)
        {
        }

        reference operator*() const { return (*collection)[pos]; }

        ProxyIterator operator++(int)
        {
            auto r = *this;
            ++(*this);
            return r;
        }

        ProxyIterator operator--(int)
        {
            auto r = *this;
            --(*this);
            return r;
        }

        ProxyIterator& operator++()
        {
            ++pos;
            return *this;
        }

        ProxyIterator& operator--()
        {
            --pos;
            return *this;
        }

        reference operator[](difference_type n) const { return *(*this + n); }

        ProxyIterator& operator+=(difference_type n)
        {
            pos += n;
            return *this;
        }

        ProxyIterator& operator-=(difference_type n)
        {
            pos -= n;
            return *this;
        }

        friend ProxyIterator
        operator+(const ProxyIterator& lhs, difference_type rhs)
        {
            return ProxyIterator(*lhs.collection, lhs.pos + rhs);
        }

        friend ProxyIterator
        operator+(difference_type lhs, const ProxyIterator& rhs)
        {
            return ProxyIterator(*rhs.collection, rhs.pos + lhs);
        }

        friend ProxyIterator
        operator-(const ProxyIterator& lhs, difference_type rhs)
        {
            return ProxyIterator(*lhs.collection, lhs.pos - rhs);
        }

        friend difference_type
        operator-(const ProxyIterator& lhs, const ProxyIterator& rhs)
        {
            return lhs.pos - rhs.pos;
        }

        friend bool
        operator==(const ProxyIterator& lhs, const ProxyIterator& rhs)
        {
            assert(lhs.collection == rhs.collection);
            return lhs.pos == rhs.pos;
        }

        friend auto
        operator<=>(const ProxyIterator& lhs, const ProxyIterator& rhs)
        {
            assert(lhs.collection == rhs.collection);
            return lhs.pos <=> rhs.pos;
        }
    };

public:
    auto begin() const
    {
        static_assert(std::random_access_iterator<ProxyIterator>);
        return ProxyIterator(*static_cast<const T*>(this), 0);
    }

    auto end() const
    {
        static_assert(std::random_access_iterator<ProxyIterator>);
        return ProxyIterator(*static_cast<const T*>(this), size());
    }

    std::size_t size() const { return static_cast<const T*>(this)->size(); }
};

} // namespace downward

#endif
