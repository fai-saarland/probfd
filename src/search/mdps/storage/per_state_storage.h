#pragma once

#include "../../algorithms/segmented_vector.h"
#include "../types.h"
#include "../value_type.h"

#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace probabilistic {

/// Namespace dedicated to commonly used storage and container types.
namespace storage {

using size_type = std::size_t;

template <typename T, typename Alloc>
struct resizing_vector : public std::vector<T, Alloc> {
    explicit resizing_vector(
        const T& default_value = T(),
        const Alloc& allocator = Alloc())
        : std::vector<T, Alloc>(allocator)
        , default_value_(default_value)
    {
    }

    typename std::vector<T, Alloc>::const_reference
    operator[](size_type idx) const = delete;

    typename std::vector<T, Alloc>::reference operator[](size_type idx)
    {
        if (idx >= this->size()) {
            this->resize(idx + 1, default_value_);
        }
        return std::vector<T, Alloc>::operator[](idx);
    }

private:
    const T default_value_;
};

template <typename T, typename Alloc>
class HashMap {
public:
    using Hash = std::hash<size_type>;
    using Equal = std::equal_to<size_type>;
    using Allocator = std::allocator<std::pair<const size_type, T>>;
    using reference = T&;
    using internal_storage =
        std::unordered_map<size_type, T, Hash, Equal, Allocator>;
    using iterator = typename internal_storage::iterator;

    HashMap(const T& default_value, const Alloc&)
        : store_(1024, Hash(), Equal(), Allocator())
        , default_value_(default_value)
    {
    }

    T& operator[](size_type idx)
    {
        auto res = store_.emplace(idx, default_value_);
        return res.first->second;
    }

    bool contains(size_type idx) const
    {
        return store_.find(idx) != store_.end();
    }

    size_type size() const { return store_.size(); }

    bool empty() const { return store_.empty(); }

    void clear() { store_.clear(); }

    iterator begin() { return store_.begin(); }

    iterator end() { return store_.end(); }

    iterator erase(iterator it) { return store_.erase(it); }

private:
    internal_storage store_;
    const T default_value_;
};

template <template <typename...> class Container>
struct is_container_persistent : std::false_type {
};

template <>
struct is_container_persistent<HashMap> : std::true_type {
};

template <>
struct is_container_persistent<segmented_vector::DynamicSegmentedVector>
    : std::true_type {
};

namespace internal {

template <
    typename T,
    typename StateIdMap,
    typename Alloc,
    template <typename, typename>
    class Container>
class PerStateStorage {
public:
    using IsPersistent = is_container_persistent<Container>;
    using internal_storage = Container<T, Alloc>;
    using reference = typename internal_storage::reference;

    explicit PerStateStorage(
        const T& default_value = T(),
        const Alloc& allocator = Alloc())
        : store_(default_value, allocator)
    {
    }

    reference operator[](const StateID& stateid) { return store_[stateid]; }

    bool contains(const StateID& stateid) const
    {
        return store_.contains(stateid);
    }

    bool empty() const { return size() == 0; }

    size_type size() const { return store_.size(); }

    void clear() { store_.clear(); }

protected:
    internal_storage store_;
};

} // namespace internal

template <
    typename T,
    typename StateIdMap = void,
    typename Alloc = std::allocator<T>>
using PersistentPerStateStorage = internal::PerStateStorage<
    T,
    StateIdMap,
    Alloc,
    segmented_vector::DynamicSegmentedVector>;

template <
    typename T,
    typename StateIdMap = void,
    typename Alloc = std::allocator<T>>
class PerStateStorage : public PersistentPerStateStorage<T, StateIdMap, Alloc> {
public:
    using
        typename PersistentPerStateStorage<T, StateIdMap, Alloc>::IsPersistent;
    using PersistentPerStateStorage<T, StateIdMap, Alloc>::
        PersistentPerStateStorage;
};

template <typename StateIdMap, typename Alloc>
class PerStateStorage<bool, StateIdMap, Alloc>
    : public internal::
          PerStateStorage<bool, StateIdMap, Alloc, resizing_vector> {
public:
    using typename internal::
        PerStateStorage<bool, StateIdMap, Alloc, resizing_vector>::IsPersistent;
    using internal::PerStateStorage<bool, StateIdMap, Alloc, resizing_vector>::
        PerStateStorage;
};

template <typename StateIdMap, typename Alloc>
class PerStateStorage<value_type::value_t, StateIdMap, Alloc>
    : public internal::PerStateStorage<
          value_type::value_t,
          StateIdMap,
          Alloc,
          resizing_vector> {
public:
    using typename internal::PerStateStorage<
        value_type::value_t,
        StateIdMap,
        Alloc,
        resizing_vector>::IsPersistent;
    using internal::PerStateStorage<
        value_type::value_t,
        StateIdMap,
        Alloc,
        resizing_vector>::PerStateStorage;
};

template <
    typename T,
    typename StateIdMap = void,
    typename Alloc = std::allocator<T>>
class StateHashMap
    : public internal::PerStateStorage<T, StateIdMap, Alloc, HashMap> {
public:
    using typename internal::PerStateStorage<T, StateIdMap, Alloc, HashMap>::
        IsPersistent;
    using internal::PerStateStorage<T, StateIdMap, Alloc, HashMap>::
        PerStateStorage;
    using internal_hash_map = typename internal::
        PerStateStorage<T, StateIdMap, Alloc, HashMap>::internal_storage;
    using iterator = typename internal_hash_map::iterator;

    iterator begin() { return this->store_.begin(); }

    iterator end() { return this->store_.end(); }

    iterator erase(iterator it) { return this->store_.erase(it); }
};

template <typename State>
using StateHashSet = std::unordered_set<State>;

using StateIDHashSet = std::unordered_set<StateID>;

} // namespace storage
} // namespace probabilistic
