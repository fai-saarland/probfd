#pragma once

#include <cassert>
#include <memory>
#include <vector>

namespace probfd::storage {

template <std::size_t SEGMENT_SIZE = 16384>
class SegmentedMemoryPool {
    std::size_t space_left_ = 0;
    void* current_ = nullptr;
    std::vector<void*> segments_;

public:
    SegmentedMemoryPool() = default;

    ~SegmentedMemoryPool()
    {
        for (void* segment : segments_) {
            delete[] (reinterpret_cast<char*>(segment));
        }
    }

    template <typename T>
    T* allocate(const std::size_t elements)
    {
        const std::size_t size = elements * sizeof(T);

        assert(size <= SEGMENT_SIZE);

        if (!std::align(alignof(T), size, current_, space_left_)) {
            space_left_ = SEGMENT_SIZE;
            current_ = ::operator new[](SEGMENT_SIZE);
            segments_.push_back(current_);
        }

        T* res = reinterpret_cast<T*>(current_);
        current_ = reinterpret_cast<char*>(current_) + size;
        space_left_ -= size;
        return res;
    }

    [[nodiscard]]
    std::size_t size_in_bytes() const
    {
        return segments_.size() * SEGMENT_SIZE;
    }
};

} // namespace probfd::storage