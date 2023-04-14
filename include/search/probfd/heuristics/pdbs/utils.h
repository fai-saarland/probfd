#ifndef PROBFD_HEURISTICS_PDBS_UTILS_H
#define PROBFD_HEURISTICS_PDBS_UTILS_H

#include "probfd/heuristics/pdbs/types.h"

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace probfd {
namespace heuristics {
namespace pdbs {

inline Pattern extended_pattern(const Pattern& pattern, int add_var)
{
    assert(!utils::contains(pattern, add_var));

    Pattern added;
    added.reserve(pattern.size() + 1);
    auto it = std::upper_bound(pattern.begin(), pattern.end(), add_var);
    added.insert(added.end(), pattern.begin(), it);
    added.emplace_back(add_var);
    added.insert(added.end(), it, pattern.end());

    return added;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__