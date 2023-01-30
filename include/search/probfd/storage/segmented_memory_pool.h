#pragma once

#include <cassert>
#include <memory>
#include <vector>

namespace probfd {
namespace storage {

template <std::size_t SEGMENT_SIZE = 16384>
class SegmentedMemoryPool {
    std::size_t space_left = 0;
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

        if (!std::align(alignof(T), size, current_, space_left)) {
            space_left = SEGMENT_SIZE;
            current_ = ::operator new[](SEGMENT_SIZE);
            segments_.push_back(current_);
        }

        T* res = reinterpret_cast<T*>(current_);
        current_ = reinterpret_cast<char*>(current_) + size;
        space_left -= size;
        return res;
    }

    std::size_t size_in_bytes() const
    {
        return segments_.size() * SEGMENT_SIZE;
    }
};

} // namespace storage
} // namespace probfd