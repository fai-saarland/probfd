#ifndef PROBFD_HEURISTICS_CARTESIAN_SPLIT_H
#define PROBFD_HEURISTICS_CARTESIAN_SPLIT_H

#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

struct Split {
    int var_id;
    std::vector<int> values;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_CARTESIAN_SPLIT_H
