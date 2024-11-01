#ifndef PROBFD_ITERATORS_H
#define PROBFD_ITERATORS_H

#include <iterator>
#include <utility>

/// Namespace dedicated to custom iterators.
namespace probfd::iterators {

struct discarding_output_iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = void;
    using pointer = void;
    using reference = void;
    using iterator_category = std::output_iterator_tag;

    /* no-op assignment */
    template <typename T>
    discarding_output_iterator& operator=(T const&)
    {
        return *this;
    }

    discarding_output_iterator& operator++() { return *this; }

    discarding_output_iterator operator++(int) { return *this; }

    discarding_output_iterator& operator*() { return *this; }
};

} // namespace probfd::iterators

#endif // PROBFD_ITERATORS_H
