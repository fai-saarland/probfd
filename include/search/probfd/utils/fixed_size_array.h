#ifndef PROBFD_UTILS_FIXED_SIZE_ARRAY_H
#define PROBFD_UTILS_FIXED_SIZE_ARRAY_H

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <ranges>

namespace probfd {

struct construct_from_range_t {
    explicit construct_from_range_t() = default;
};

inline constexpr construct_from_range_t construct_from_range;

template <typename T>
class FixedSizeArray {
    std::size_t size_;
    std::unique_ptr<T[]> array_;

public:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::contiguous_iterator_tag;

    explicit FixedSizeArray(std::size_t size = 0)
        requires std::default_initializable<T>
        : size_(size)
        , array_(new T[size_])
    {
    }

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    explicit FixedSizeArray(std::size_t size, Args&&... args)
        : size_(size)
        , array_(static_cast<T*>(::operator new[](
              size_ * sizeof(T),
              std::align_val_t{alignof(T)})))
    {
        for (auto it = begin(), e = end(); it != e; ++it) {
            std::construct_at<T>(it, std::forward<Args>(args)...);
        }
    }

    template <std::ranges::input_range Range>
        requires requires(Range&& r) {
            {
                std::ranges::size(r)
            } -> std::convertible_to<std::size_t>;
        } &&
                     std::constructible_from<
                         T,
                         std::ranges::range_const_reference_t<Range>>
    explicit FixedSizeArray(construct_from_range_t, Range&& range)
        : size_(std::ranges::size(range))
        , array_(static_cast<T*>(::operator new[](
              size_ * sizeof(T),
              std::align_val_t{alignof(T)})))
    {
        auto it = begin(), e = end();
        auto rit = std::ranges::begin(range);
        for (; it != e; ++it, ++rit) {
            std::construct_at<T>(it, std::forward(*rit));
        }
    }

    template <std::ranges::input_range Range>
        requires requires(Range&& r) {
            {
                std::ranges::size(r)
            } -> std::convertible_to<std::size_t>;
        } && std::assignable_from<T&, std::ranges::range_value_t<Range>>
    explicit FixedSizeArray(std::from_range_t, Range&& range)
        : size_(std::ranges::size(range))
        , array_(new T[size_]{})
    {
        auto it = begin();
        auto en = end();
        auto rit = range.begin();

        for (; it != en; ++it, ++rit) {
            *it = *rit;
        }
    }

    FixedSizeArray(const FixedSizeArray& other)
        : size_(other.size_)
        , array_(new T[other.size_])
    {
        std::memcpy(begin(), other.begin(), size_);
    }

    FixedSizeArray& operator=(const FixedSizeArray& other)
    {
        size_ = other.size_;
        array_.reset(new T[other.size_]);
        std::memcpy(begin(), other.begin(), size_);
    }

    FixedSizeArray(FixedSizeArray&& other) = default;
    FixedSizeArray& operator=(FixedSizeArray&& other) = default;

    T* begin() { return array_.get(); }
    T* end() { return begin() + size_; }

    const T* begin() const { return array_.get(); }
    const T* end() const { return begin() + size_; }

    T& operator[](std::integral auto i) { return array_.get()[i]; };
    const T& operator[](std::integral auto i) const { return array_.get()[i]; };

    T& front() { return *begin(); }
    const T& front() const { return *begin(); }

    bool empty() { return size_ == 0; }
    std::size_t get_size() const { return size_; }

    template <typename T>
    friend std::ostream&
    operator<<(std::ostream& os, const FixedSizeArray<T>& array)
    {
        os << "[";
        if (array.size_ > 0) {
            os << array.front();
            for (const T& elem : array | std::views::drop(1)) {
                os << ", " << elem;
            }
        }
        return os << "]";
    }

    template <typename T>
    friend std::istream& operator>>(std::istream& in, FixedSizeArray<T>& array)
    {
        if (in.get() != '[') {
            in.setstate(std::ios::failbit);
            return in;
        }

        auto it = array.begin();
        auto end = array.end();

        if ((in >> std::ws, in.peek() != ']')) {
            if (it == end) {
                in.setstate(std::ios::failbit);
                return in;
            }

            {
                T element;
                if (!(in >> element)) return in;
                *it++ = std::move(element);
            }

            while ((in >> std::ws, in.peek() != ']')) {
                if (it == end || in.get() != ',') {
                    in.setstate(std::ios::failbit);
                    return in;
                }

                T element;
                if (!(in >> std::ws >> element)) return in;
                *it++ = std::move(element);
            }
        }

        in.ignore();
        return in;
    }
};

} // namespace probfd

#endif // PROBFD_UTILS_FIXED_SIZE_ARRAY_H
