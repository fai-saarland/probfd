#ifndef PROBFD_STORAGE_PER_STATE_STORAGE_H
#define PROBFD_STORAGE_PER_STATE_STORAGE_H

#include "probfd/utils/iterators.h"

#include "probfd/state_id.h"

#include "downward/algorithms/segmented_vector.h"

#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/// This namespace contains commonly used storage and container types.
namespace probfd::storage {

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
        if (idx >= this->size()) { this->resize(idx + 1, default_value_); }
        return std::vector<T, Alloc>::operator[](idx);
    }

    typename std::vector<T, Alloc>::const_reference
    operator[](StateID idx) const
    {
        if (idx >= this->size()) { return default_value_; }
        return std::vector<T, Alloc>::operator[](idx);
    }

private:
    const T default_value_;
};

template <class Element, class Allocator = std::allocator<Element>>
class PerStateStorage
    : public downward::segmented_vector::SegmentedVector<Element, Allocator> {
    Element default_value_;

public:
    explicit PerStateStorage(
        const Element& default_value = Element(),
        const Allocator& alloc = Allocator())
        : downward::segmented_vector::SegmentedVector<Element>(alloc)
        , default_value_(default_value)
    {
    }

    Element& operator[](size_t index)
    {
        if (index >= this->size()) { this->resize(index + 1, default_value_); }
        return downward::segmented_vector::SegmentedVector<Element, Allocator>::
        operator[](index);
    }

    const Element& operator[](size_t index) const
    {
        if (index >= this->size()) { return default_value_; }
        return downward::segmented_vector::SegmentedVector<Element, Allocator>::
        operator[](index);
    }

    [[nodiscard]]
    bool empty() const
    {
        return this->size() == 0;
    }
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

    explicit StateHashMap(
        const T& default_value = T(),
        const Hash& h = Hash(),
        const Equal& e = Equal(),
        const Alloc& a = Alloc())
        : store_(1024, h, e, a)
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

    [[nodiscard]]
    bool contains(StateID idx) const
    {
        return store_.find(idx) != store_.end();
    }

    [[nodiscard]]
    StateID size() const
    {
        return store_.size();
    }

    [[nodiscard]]
    bool empty() const
    {
        return store_.empty();
    }

    void clear() { store_.clear(); }

    iterator begin() { return store_.begin(); }

    iterator end() { return store_.end(); }

    iterator erase(iterator it) { return store_.erase(it); }

    template <class Pred>
    typename std::unordered_map<StateID, T, Hash, Equal, Alloc>::size_type
    erase_if(Pred pred)
    {
        return std::erase_if(store_, pred);
    }

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

} // namespace probfd::storage

#endif
