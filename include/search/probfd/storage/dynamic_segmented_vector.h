#ifndef PROBFD_STORAGE_DYNAMIC_SEGMENTED_VECTOR
#define PROBFD_STORAGE_DYNAMIC_SEGMENTED_VECTOR

#include "algorithms/segmented_vector.h"

namespace probfd {
namespace storage {

template <class Element, class Allocator = std::allocator<Element>>
class DynamicSegmentedVector
    : public segmented_vector::SegmentedVector<Element, Allocator> {
    Element default_value_;

public:
    using reference = Element&;

    DynamicSegmentedVector(
        const Element& default_value = Element(),
        const Allocator& alloc = Allocator())
        : segmented_vector::SegmentedVector<Element>(alloc)
        , default_value_(default_value)
    {
    }

    Element& operator[](size_t index)
    {
        if (index >= this->size()) {
            this->resize(index + 1, default_value_);
        }
        return segmented_vector::SegmentedVector<Element, Allocator>::
        operator[](index);
    }
};

} // namespace storage
} // namespace probfd

#endif