#ifndef UTILS_COLLECTIONS_H
#define UTILS_COLLECTIONS_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace utils {

template <typename T, typename A>
std::vector<T, A>
set_intersection(const std::vector<T, A>& lhs, const std::vector<T, A>& rhs)
{
    assert(std::is_sorted(lhs.begin(), lhs.end()));
    assert(std::is_sorted(rhs.begin(), rhs.end()));

    std::vector<T, A> intersection;

    set_intersection(
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        rhs.end(),
        back_inserter(intersection));

    return intersection;
}

template <typename T, typename A>
std::vector<T, A>
set_union(const std::vector<T, A>& lhs, const std::vector<T, A>& rhs)
{
    assert(std::is_sorted(lhs.begin(), lhs.end()));
    assert(std::is_sorted(rhs.begin(), rhs.end()));

    std::vector<T, A> _union;

    set_union(
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        rhs.end(),
        back_inserter(_union));

    return _union;
}

template <typename T, typename A>
std::vector<T, A>
set_difference(const std::vector<T, A>& lhs, const std::vector<T, A>& rhs)
{
    assert(std::is_sorted(lhs.begin(), lhs.end()));
    assert(std::is_sorted(rhs.begin(), rhs.end()));

    std::vector<T, A> _difference;

    set_difference(
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        rhs.end(),
        back_inserter(_difference));

    return _difference;
}

template <typename T, typename A>
void insert_set(std::vector<T, A>& lhs, T element)
{
    assert(std::is_sorted(lhs.begin(), lhs.end()));

    auto it = std::lower_bound(lhs.begin(), lhs.end(), element);
    if (it == lhs.end() || *it != element) {
        lhs.insert(it, std::move(element));
    }
}

template <typename T, typename A>
void insert_set(std::vector<T, A>& lhs, const std::vector<T, A>& rhs)
{
    for (const auto& element : rhs) {
        insert_set(lhs, element);
    }
}

template <typename T, typename A>
bool contains(const std::vector<T, A>& vec, const T& val)
{
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

template <typename T, typename C, typename A>
bool contains(const std::set<T, C, A>& set, const T& val)
{
    return set.find(val) != set.end();
}

template <typename T, typename H, typename KEq, typename A>
bool contains(const std::unordered_set<T, H, KEq, A>& set, const T& val)
{
    return set.find(val) != set.end();
}

template <typename K, typename V, typename H, typename KEq, typename A>
bool contains_key(const std::unordered_map<K, V, H, KEq, A>& map, const K& key)
{
    return map.find(key) != map.end();
}

template <typename K, typename V, typename C, typename A>
bool contains_key(const std::map<K, V, C, A>& map, const K& key)
{
    return map.find(key) != map.end();
}

template <class T, class A>
extern void sort_unique(std::vector<T, A>& vec)
{
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

template <class T>
extern bool is_unique(const std::vector<T>& values)
{
    std::vector<T> temp(values.begin(), values.end());
    std::sort(temp.begin(), temp.end());
    return std::adjacent_find(temp.begin(), temp.end()) == temp.end();
}

template <class T>
extern bool is_sorted_unique(const std::vector<T>& values)
{
    for (size_t i = 1; i < values.size(); ++i) {
        if (values[i - 1] >= values[i]) return false;
    }
    return true;
}

template <class T>
bool in_bounds(int index, const T& container)
{
    return index >= 0 && static_cast<size_t>(index) < container.size();
}

template <class T>
bool in_bounds(long index, const T& container)
{
    return index >= 0 && static_cast<size_t>(index) < container.size();
}

template <class T>
bool in_bounds(size_t index, const T& container)
{
    return index < container.size();
}

inline std::vector<int> insert(std::vector<int> pattern, int add_var)
{
    assert(!utils::contains(pattern, add_var));
    auto it = std::lower_bound(pattern.begin(), pattern.end(), add_var);
    pattern.insert(it, add_var);
    return pattern;
}

template <typename T>
T swap_and_pop_from_vector(std::vector<T>& vec, size_t pos)
{
    assert(in_bounds(pos, vec));
    T element = vec[pos];
    std::swap(vec[pos], vec.back());
    vec.pop_back();
    return element;
}

template <class T>
void release_vector_memory(std::vector<T>& vec)
{
    std::vector<T>().swap(vec);
}

template <class KeyType, class ValueType>
ValueType get_value_or_default(
    const std::unordered_map<KeyType, ValueType>& dict,
    const KeyType& key,
    const ValueType& default_value)
{
    auto it = dict.find(key);
    if (it != dict.end()) {
        return it->second;
    }
    return default_value;
}

template <typename ElemTo, typename Collection, typename MapFunc>
std::vector<ElemTo> map_vector(const Collection& collection, MapFunc map_func)
{
    std::vector<ElemTo> transformed;
    transformed.reserve(collection.size());
    std::transform(
        begin(collection),
        end(collection),
        std::back_inserter(transformed),
        map_func);
    return transformed;
}

template <typename T, typename Collection>
std::vector<T> sorted(Collection&& collection)
{
    std::vector<T> vec(std::forward<Collection>(collection));
    std::sort(vec.begin(), vec.end());
    return vec;
}

template <typename T>
int estimate_vector_bytes(int num_elements)
{
    /*
      This estimate is based on a study of the C++ standard library
      that shipped with gcc around the year 2017. It does not claim to
      be accurate and may certainly be inaccurate for other compilers
      or compiler versions.
    */
    int size = 0;
    size += 2 * sizeof(void*);        // overhead for dynamic memory management
    size += sizeof(std::vector<T>);   // size of empty vector
    size += num_elements * sizeof(T); // size of actual entries
    return size;
}

template <typename T>
int _estimate_hash_table_bytes(int num_entries)
{
    /*
      The same comments as for estimate_vector_bytes apply.
      Additionally, there may be alignment issues, especially on
      64-bit systems, that make this estimate too optimistic for
      certain cases.
    */

    assert(num_entries < (1 << 28));
    /*
      Having num_entries < 2^28 is necessary but not sufficient for
      the result value to not overflow. If we ever change this
      function to support larger data structures (using a size_t
      return value), we must update the list of bounds below (taken
      from the gcc library source).
    */
    int num_buckets = 0;
    const auto bounds = {
        2,       5,        11,       23,       47,        97,       199,
        409,     823,      1741,     3469,     6949,      14033,    28411,
        57557,   116731,   236897,   480881,   976369,    1982627,  4026031,
        8175383, 16601593, 33712729, 68460391, 139022417, 282312799};

    for (int bound : bounds) {
        if (num_entries < bound) {
            num_buckets = bound;
            break;
        }
    }

    int size = 0;
    size += 2 * sizeof(void*); // overhead for dynamic memory management
    size += sizeof(T);         // empty container
    using Entry = typename T::value_type;
    size += num_entries * sizeof(Entry);  // actual entries
    size += num_entries * sizeof(Entry*); // pointer to values
    size += num_entries * sizeof(void*);  // pointer to next node
    size += num_buckets * sizeof(void*);  // pointer to next bucket
    return size;
}

template <typename T>
int estimate_unordered_set_bytes(int num_entries)
{
    // See comments for _estimate_hash_table_bytes.
    return _estimate_hash_table_bytes<std::unordered_set<T>>(num_entries);
}

template <typename Key, typename Value>
int estimate_unordered_map_bytes(int num_entries)
{
    // See comments for _estimate_hash_table_bytes.
    return _estimate_hash_table_bytes<std::unordered_map<Key, Value>>(
        num_entries);
}
} // namespace utils

#endif
