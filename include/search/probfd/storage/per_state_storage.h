#ifndef PROBFD_STORAGE_PER_STATE_STORAGE_H
#define PROBFD_STORAGE_PER_STATE_STORAGE_H

#include "probfd/types.h"
#include "probfd/value_type.h"

#include "algorithms/segmented_vector.h"

#include "utils/iterators.h"

#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace probfd {

/// Namespace dedicated to commonly used storage and container types.
namespace storage {

template <typename T, typename Alloc>
struct resizing_vector : public std::vector<T, Alloc> {
    explicit resizing_vector(
        const T& default_value = T(),
        const Alloc& allocator = Alloc())
        : std::vector<T, Alloc>(allocator)
        , default_value_(default_value)
    {
    }

    typename std::vector<T, Alloc>::reference operator[](StateID idx)
    {
        if (idx >= this->size()) {
            this->resize(idx + 1, default_value_);
        }
        return std::vector<T, Alloc>::operator[](idx);
    }

    typename std::vector<T, Alloc>::const_reference
    operator[](StateID idx) const
    {
        if (idx >= this->size()) {
            return default_value_;
        }
        return std::vector<T, Alloc>::operator[](idx);
    }

private:
    const T default_value_;
};

template <class Element, class Allocator = std::allocator<Element>>
class PerStateStorage
    : public segmented_vector::SegmentedVector<Element, Allocator> {
    Element default_value_;

public:
    PerStateStorage(
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

    const Element& operator[](size_t index) const
    {
        if (index >= this->size()) {
            return default_value_;
        }
        return segmented_vector::SegmentedVector<Element, Allocator>::
        operator[](index);
    }

    bool empty() const { return this->size() == 0; }
};

template <
    typename T,
    typename Hash = std::hash<StateID>,
    typename Equal = std::equal_to<StateID>,
    typename Alloc = std::allocator<std::pair<const StateID, T>>>
class StateHashMap {
public:
    using iterator =
        typename std::unordered_map<StateID, T, Hash, Equal, Alloc>::iterator;

    StateHashMap(
        const T& default_value = T(),
        const Hash& H = Hash(),
        const Equal& E = Equal(),
        const Alloc& A = Alloc())
        : store_(1024, H, E, A)
        , default_value_(default_value)
    {
    }

    T& operator[](StateID idx)
    {
        auto res = store_.emplace(idx, default_value_);
        return res.first->second;
    }

    const T& operator[](StateID idx) const
    {
        auto it = store_.find(idx);
        return it != store_.end() ? it->second : default_value_;
    }

    bool contains(StateID idx) const
    {
        return store_.find(idx) != store_.end();
    }

    StateID size() const { return store_.size(); }

    bool empty() const { return store_.empty(); }

    void clear() { store_.clear(); }

    iterator begin() { return store_.begin(); }

    iterator end() { return store_.end(); }

    iterator erase(iterator it) { return store_.erase(it); }

private:
    std::unordered_map<StateID, T, Hash, Equal, Alloc> store_;
    const T default_value_;
};

template <typename Alloc>
class PerStateStorage<bool, Alloc> : public resizing_vector<bool, Alloc> {
public:
    using resizing_vector<bool, Alloc>::resizing_vector;
};

template <typename State>
using StateHashSet = std::unordered_set<State>;

using StateIDHashSet = std::unordered_set<StateID>;

} // namespace storage
} // namespace probfd

namespace utils {
template <>
class set_output_iterator<probfd::storage::PerStateStorage<bool>> {
    probfd::storage::PerStateStorage<bool>& store;

public:
    set_output_iterator(probfd::storage::PerStateStorage<bool>& store)
        : store(store)
    {
    }

    probfd::storage::PerStateStorage<bool>::reference
    operator=(const ::probfd::StateID& id)
    {
        return store[id] = true;
    }

    set_output_iterator& operator++() { return *this; }

    set_output_iterator operator++(int) { return *this; }

    set_output_iterator& operator*() { return *this; }
};
} // namespace utils

#endif
