#pragma once

#include <cassert>
#include <queue>

namespace segmented_array_store {

template<typename T>
class SegmentedArrayStore {
public:
    static constexpr const std::size_t SEGMENT_ELEMENTS = 16384;
    static constexpr const std::size_t SEGMENT_SIZE =
        (SEGMENT_ELEMENTS * sizeof(T));

    explicit SegmentedArrayStore()
    {
    }

    ~SegmentedArrayStore()
    {
        while (!segments_.empty()) {
            delete[](segments_.front());
            segments_.pop();
        }
    }

    T* allocate(const unsigned size)
    {
        assert(size <= SEGMENT_ELEMENTS);
        if (size > segments_left_) {
            segments_left_ = SEGMENT_ELEMENTS;
            current_ = new char[SEGMENT_SIZE];
            segments_.push(current_);
        }
        T* res = reinterpret_cast<T*>(current_);
        current_ += (size * sizeof(T));
        segments_left_ -= size;
        return res;
    }

    std::size_t size_in_bytes() const
    {
        return (segments_.size() * SEGMENT_ELEMENTS)
            * sizeof(T);
    }

private:
    std::size_t segments_left_ = 0;
    char* current_ = nullptr;
    std::queue<char*> segments_;
};

} // namespace segmented_array_store
