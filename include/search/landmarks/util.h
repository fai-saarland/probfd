#ifndef LANDMARKS_UTIL_H
#define LANDMARKS_UTIL_H

#include <cstddef>
#include <vector>
#include <unordered_map>

struct GlobalCondition;
class GlobalOperator;
class LandmarkNode;

class hash_pointer {
public:
    size_t operator()(const void *p) const {
        //return size_t(reinterpret_cast<int>(p));
        std::hash<const void *> my_hash_class;
        return my_hash_class(p);
    }
};

bool _possibly_fires(const std::vector<GlobalCondition> &conditions,
                     const std::vector<std::vector<int> > &lvl_var);

std::unordered_map<int, int> _intersect(
    const std::unordered_map<int, int> &a,
    const std::unordered_map<int, int> &b);

bool _possibly_reaches_lm(const GlobalOperator &o,
                          const std::vector<std::vector<int> > &lvl_var,
                          const LandmarkNode *lmp);

#endif
