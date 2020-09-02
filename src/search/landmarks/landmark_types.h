#ifndef LANDMARKS_LANDMARK_TYPES_H
#define LANDMARKS_LANDMARK_TYPES_H

#include <utility>
#include <unordered_set>

struct hash_int_pair {
    size_t operator()(const std::pair<int, int> &key) const {
        return size_t(key.first * 1337 + key.second);
    }
};

typedef std::unordered_set<std::pair<int, int>, hash_int_pair> lm_set;

#endif
