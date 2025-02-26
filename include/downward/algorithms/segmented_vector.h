#ifndef ALGORITHMS_SEGMENTED_VECTOR_H
#define ALGORITHMS_SEGMENTED_VECTOR_H

#include <algorithm>
#include <cassert>
#include <compare>
#include <iostream>
#include <vector>

/*
  SegmentedVector is a vector-like class with the following advantages over
  vector:
    1. Resizing has no memory spike. (*)
    2. Should work more nicely with fragmented memory because data is
       partitioned into fixed-size chunks of size SEGMENT_BYTES.
    3. Overallocation is only additive (by SEGMENT_BYTES), not multiplicative
       as in vector. (*)
    4. References stay stable forever, so there is no need to be careful about
       invalidating references upon growing the vector.

  (*) Assumes that the size of the "segments" vector can be neglected, which is
  true if SEGMENT_BYTES isn't chosen too small. For example, with 1 GB of data
  and SEGMENT_BYTES = 8192, we can have 131072 segments.

  The main disadvantage to vector is that there is an additional indirection
  for each lookup, but we hope that the first lookup will usually hit the cache.
  The implementation is basically identical to that of deque (at least the
  g++ version), but with the advantage that we can control SEGMENT_BYTES. A
  test on all optimal planning instances with several planner configurations
  showed a modest advantage over deque.

  The class can also be used as a simple "memory pool" to reduce allocation
  costs (time and memory) when allocating many objects of the same type.

  SegmentedArrayVector is a similar class that can be used for compactly
  storing many fixed-size arrays. It's essentially a variant of SegmentedVector
  where the size of the stored data is only known at runtime, not at compile
  time.
*/

/*
  There is currently a significant amount of duplication between the
  two classes. We decided to live with this for the time being,
  but this could certainly be made prettier.
*/

/*
  For documentation on classes relevant to storing and working with registered
  states see the file state_registry.h.
*/

namespace segmented_vector {
template <class Entry, class Allocator = std::allocator<Entry>>
class SegmentedVector {
    using ATraits = std::allocator_traits<Allocator>;
    using EntryAllocator = typename ATraits::template rebind_alloc<Entry>;

    static const size_t SEGMENT_BYTES = 8192;

    static const size_t SEGMENT_ELEMENTS = (SEGMENT_BYTES / sizeof(Entry)) >= 1
                                               ? (SEGMENT_BYTES / sizeof(Entry))
                                               : 1;

    EntryAllocator entry_allocator;

    std::vector<Entry*> segments;
    size_t the_size;

    size_t get_segment(size_t index) const { return index / SEGMENT_ELEMENTS; }

    size_t get_offset(size_t index) const { return index % SEGMENT_ELEMENTS; }

    void add_segment()
    {
        Entry* new_segment = entry_allocator.allocate(SEGMENT_ELEMENTS);
        segments.push_back(new_segment);
    }

    SegmentedVector(const SegmentedVector<Entry>&) = delete;
    SegmentedVector& operator=(const SegmentedVector<Entry>&) = delete;

    template <bool is_const>
    class sviterator {
        std::vector<Entry*>::iterator current_segment;
        Entry* current_entry = nullptr;
        Entry* first_entry = nullptr;
        Entry* last_entry = nullptr;

    public:
        using value_type = std::conditional_t<is_const, const Entry, Entry>;
        using pointer = std::conditional_t<is_const, const Entry*, Entry*>;
        using reference = std::conditional_t<is_const, const Entry&, Entry&>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        sviterator() = default;

        sviterator(
            std::vector<Entry*>::iterator current_segment,
            Entry* current_entry)
            : current_segment(current_segment)
            , current_entry(current_entry)
            , first_entry(*current_segment)
            , last_entry(first_entry + SEGMENT_ELEMENTS)
        {
        }

        reference operator*() const { return *current_entry; }
        pointer operator->() { return current_entry; };

        sviterator& operator++()
        {
            ++current_entry;

            if (current_entry == last_entry) {
                ++current_segment;
                first_entry = *current_segment;
                last_entry = *current_segment + SEGMENT_ELEMENTS;
                current_entry = first_entry;
            }

            return *this;
        }

        sviterator& operator--()
        {
            if (current_entry == first_entry) {
                --current_segment;
                first_entry = *current_segment;
                last_entry = *current_segment + SEGMENT_ELEMENTS;
                current_entry = last_entry;
            }

            --current_entry;

            return *this;
        }

        friend difference_type
        operator-(const sviterator& lhs, const sviterator& rhs)
        {
            return (lhs.current_segment - rhs.current_segment) *
                       SEGMENT_ELEMENTS +
                   (lhs.current_entry - rhs.current_entry);
        }

        sviterator& operator+=(int n)
        {
            int offset = n + (current_entry - first_entry);

            if (offset > 0 && offset < SEGMENT_ELEMENTS) {
                current_entry += n;
                return *this;
            }

            const difference_type segment_offset =
                offset > 0
                    ? offset / difference_type(SEGMENT_ELEMENTS)
                    : -difference_type((-offset - 1) / SEGMENT_ELEMENTS) - 1;

            current_segment += segment_offset;
            first_entry = *current_segment;
            last_entry = *current_segment + SEGMENT_ELEMENTS;
            current_entry =
                first_entry +
                (offset - segment_offset * difference_type(SEGMENT_ELEMENTS));

            return *this;
        }

        sviterator operator++(int)
        {
            auto r = *this;
            ++(*this);
            return r;
        }

        sviterator operator--(int)
        {
            auto r = *this;
            --(*this);
            return r;
        }

        friend sviterator operator+(const sviterator& it, int n)
        {
            auto r = it;
            r += n;
            return r;
        }

        friend sviterator operator+(int n, const sviterator& it)
        {
            auto r = it;
            r += n;
            return r;
        }

        friend sviterator operator-(const sviterator& it, int n)
        {
            auto r = it;
            r -= n;
            return r;
        }

        sviterator& operator-=(int n) { return (*this) += -n; }

        reference operator[](int n) { return *(*this + n); };

        friend auto operator<=>(const sviterator&, const sviterator&) = default;

        friend void swap(sviterator& left, sviterator& right)
        {
            using std::swap;
            swap(left.current_segment, right.current_segment);
            swap(left.current_entry, right.current_entry);
        }
    };

public:
    using iterator = sviterator<false>;
    using const_iterator = sviterator<true>;

    SegmentedVector()
        : the_size(0)
    {
        // Add an initial segment to make iterator implementation easier
        add_segment();
    }

    SegmentedVector(std::size_t size, const Entry& entry = Entry())
        : SegmentedVector()
    {
        while (size > the_size) {
            push_back(entry);
        }
    }

    SegmentedVector(const EntryAllocator& allocator_)
        : entry_allocator(allocator_)
        , the_size(0)
    {
        add_segment();
    }

    ~SegmentedVector()
    {
        for (size_t i = 0; i < the_size; ++i) {
            ATraits::destroy(entry_allocator, &operator[](i));
        }
        for (size_t segment = 0; segment < segments.size(); ++segment) {
            entry_allocator.deallocate(segments[segment], SEGMENT_ELEMENTS);
        }
    }

    Entry& operator[](size_t index)
    {
        assert(index < the_size);
        size_t segment = get_segment(index);
        size_t offset = get_offset(index);
        return segments[segment][offset];
    }

    const Entry& operator[](size_t index) const
    {
        assert(index < the_size);
        size_t segment = get_segment(index);
        size_t offset = get_offset(index);
        return segments[segment][offset];
    }

    size_t size() const { return the_size; }

    void push_back(const Entry& entry)
    {
        size_t segment = get_segment(the_size);
        size_t offset = get_offset(the_size);
        if (segment == segments.size() - 1) {
            assert(offset == 0);
            // Must add a new segment.
            add_segment();
        }
        ATraits::construct(entry_allocator, segments[segment] + offset, entry);
        ++the_size;
    }

    void pop_back()
    {
        ATraits::destroy(entry_allocator, &operator[](the_size - 1));
        --the_size;
        /*
          If the removed element was the last in its segment, the segment
          is not removed (memory is not deallocated). This way a subsequent
          push_back does not have to allocate the memory again.
        */
    }

    void resize(size_t new_size, Entry entry = Entry())
    {
        while (new_size < the_size) {
            pop_back();
        }
        while (new_size > the_size) {
            push_back(entry);
        }
    }

    void clear()
    {
        for (size_t i = 0; i < the_size; ++i) {
            ATraits::destroy(entry_allocator, &operator[](i));
        }

        the_size = 0;
    }

    iterator begin() { return iterator(segments.begin(), segments.front()); }

    iterator end()
    {
        return iterator(segments.end() - 1, *(segments.end() - 1));
    }

    const_iterator begin() const
    {
        return const_iterator(segments.begin(), *segments.front());
    }

    const_iterator end() const
    {
        return const_iterator(segments.end() - 1, *(segments.end() - 1));
    }
};

template <class Element, class Allocator = std::allocator<Element>>
class SegmentedArrayVector {
    using ATraits = std::allocator_traits<Allocator>;
    using ElementAllocator = typename ATraits::template rebind_alloc<Element>;

    static const size_t SEGMENT_BYTES = 8192;

    const size_t elements_per_array;
    const size_t arrays_per_segment;
    const size_t elements_per_segment;

    ElementAllocator element_allocator;

    std::vector<Element*> segments;
    size_t the_size;

    size_t get_segment(size_t index) const
    {
        return index / arrays_per_segment;
    }

    size_t get_offset(size_t index) const
    {
        return (index % arrays_per_segment) * elements_per_array;
    }

    void add_segment()
    {
        Element* new_segment = element_allocator.allocate(elements_per_segment);
        segments.push_back(new_segment);
    }

    SegmentedArrayVector(const SegmentedArrayVector<Element>&) = delete;
    SegmentedArrayVector&
    operator=(const SegmentedArrayVector<Element>&) = delete;

public:
    SegmentedArrayVector(size_t elements_per_array_)
        : elements_per_array(elements_per_array_)
        , arrays_per_segment(
              std::max(
                  SEGMENT_BYTES / (elements_per_array * sizeof(Element)),
                  size_t(1)))
        , elements_per_segment(elements_per_array * arrays_per_segment)
        , the_size(0)
    {
    }

    SegmentedArrayVector(
        size_t elements_per_array_,
        const ElementAllocator& allocator_)
        : element_allocator(allocator_)
        , elements_per_array(elements_per_array_)
        , arrays_per_segment(
              std::max(
                  SEGMENT_BYTES / (elements_per_array * sizeof(Element)),
                  size_t(1)))
        , elements_per_segment(elements_per_array * arrays_per_segment)
        , the_size(0)
    {
    }

    ~SegmentedArrayVector()
    {
        for (size_t i = 0; i < the_size; ++i) {
            for (size_t offset = 0; offset < elements_per_array; ++offset) {
                ATraits::destroy(element_allocator, operator[](i) + offset);
            }
        }
        for (size_t i = 0; i < segments.size(); ++i) {
            element_allocator.deallocate(segments[i], elements_per_segment);
        }
    }

    Element* operator[](size_t index)
    {
        assert(index < the_size);
        size_t segment = get_segment(index);
        size_t offset = get_offset(index);
        return segments[segment] + offset;
    }

    const Element* operator[](size_t index) const
    {
        assert(index < the_size);
        size_t segment = get_segment(index);
        size_t offset = get_offset(index);
        return segments[segment] + offset;
    }

    size_t size() const { return the_size; }

    void push_back(const Element* entry)
    {
        size_t segment = get_segment(the_size);
        size_t offset = get_offset(the_size);
        if (segment == segments.size()) {
            assert(offset == 0);
            // Must add a new segment.
            add_segment();
        }
        Element* dest = segments[segment] + offset;
        for (size_t i = 0; i < elements_per_array; ++i)
            ATraits::construct(element_allocator, dest++, *entry++);
        ++the_size;
    }

    void pop_back()
    {
        for (size_t offset = 0; offset < elements_per_array; ++offset) {
            ATraits::destroy(
                element_allocator,
                operator[](the_size - 1) + offset);
        }
        --the_size;
        /*
          If the removed element was the last in its segment, the segment
          is not removed (memory is not deallocated). This way a subsequent
          push_back does not have to allocate the memory again.
        */
    }

    void resize(size_t new_size, const Element* entry)
    {
        while (new_size < the_size) {
            pop_back();
        }
        while (new_size > the_size) {
            push_back(entry);
        }
    }
};
} // namespace segmented_vector

#endif
