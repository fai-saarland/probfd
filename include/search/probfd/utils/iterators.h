#ifndef PROBFD_ITERATORS_H
#define PROBFD_ITERATORS_H

#include <iterator>
#include <utility>

namespace probfd {

/// Namespace dedicated to custom iterators.
namespace iterators {

struct discarding_output_iterator {
    using difference_type = std::ptrdiff_t;
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

} // namespace iterators
} // namespace probfd

#endif // __ITERATORS_H__
