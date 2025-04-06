#ifndef UTILS_COLLECTIONS_H
#define UTILS_COLLECTIONS_H

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <map>
#include <ranges>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace downward::utils {

template <class T>
extern void sort_unique(std::vector<T>& vec)
{
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
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
extern bool all_values_unique(const std::vector<T>& v)
{
    std::unordered_set<T> s(v.begin(), v.end());
    return s.size() == v.size();
}

template <class T>
bool in_bounds(std::signed_integral auto index, const T& container)
{
    return index >= 0 && static_cast<size_t>(index) < container.size();
}

template <class T>
bool in_bounds(std::unsigned_integral auto index, const T& container)
{
    return static_cast<size_t>(index) < container.size();
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
        std::begin(collection),
        std::end(collection),
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

template <class I1, class I2>
bool have_common_element(I1 first1, I1 last1, I2 first2, I2 last2)
{
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            ++first1;
        } else if (*first2 < *first1) {
            ++first2;
        } else {
            return true;
        }
    }

    return false;
}

template <class Range1, class Range2>
bool have_common_element(const Range1& range1, const Range2& range2)
{
    return have_common_element(
        std::begin(range1),
        std::end(range1),
        std::begin(range2),
        std::end(range2));
}

// The following are used by probfd
template <typename T, typename A>
std::vector<T, A>
merge_sorted(const std::vector<T, A>& left, const std::vector<T, A>& right)
{
    std::vector<T, A> merged;
    merged.reserve(left.size() + right.size());
    std::ranges::merge(left, right, std::back_inserter(merged));
    return merged;
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

template <std::ranges::input_range T>
bool contains(T&& range, const std::ranges::range_value_t<T>& val)
{
    return std::ranges::find(range, val) != std::ranges::end(range);
}

template <class T>
extern bool is_unique(const std::vector<T>& values)
{
    std::vector<T> temp(values.begin(), values.end());
    std::ranges::sort(temp);
    return is_sorted_unique(temp);
}

template <typename Iterator, typename Sentinel, typename T>
auto find_sorted(Iterator begin, Sentinel end, const T& elem)
{
    auto it = std::lower_bound(begin, end, elem);
    return it != end && *it == elem ? it : end;
}

template <class T>
void release_container_memory(T& container)
{
    T().swap(container);
}

} // namespace utils

#endif
