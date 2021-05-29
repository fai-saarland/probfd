#ifndef PDBS_CEGAR_UTILS_H
#define PDBS_CEGAR_UTILS_H

#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>

namespace {

template <typename T, typename A>
bool contains(const std::vector<T, A>& vec, const T& val) {
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

template <typename T, typename C, typename A>
bool contains(const std::set<T, C, A>& set, const T& val) {
    return set.find(val) != set.end();
}

template <typename T, typename H, typename KEq, typename A>
bool contains(const std::unordered_set<T, H, KEq, A>& set, const T& val) {
    return set.find(val) != set.end();
}

template <typename K, typename V, typename H, typename KEq, typename A>
bool contains(const std::unordered_map<K, V, H, KEq, A>& map, const K& key) {
    return map.find(key) != map.end();
}

}

#endif
