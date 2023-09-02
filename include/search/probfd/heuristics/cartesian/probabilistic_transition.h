#ifndef PROBFD_HEURISTICS_CARTESIAN_PROBABILISTIC_TRANSITION_H
#define PROBFD_HEURISTICS_CARTESIAN_PROBABILISTIC_TRANSITION_H

#include <cassert>
#include <compare>
#include <iostream>
#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

struct ProbabilisticTransition {
    int op_id;
    std::vector<int> target_ids; // One target for each operator effect

    ProbabilisticTransition(int op_id, std::vector<int> target_ids)
        : op_id(op_id)
        , target_ids(std::move(target_ids))
    {
        assert(!this->target_ids.empty());
    }

    friend std::ostream&
    operator<<(std::ostream& os, const ProbabilisticTransition& t)
    {
        os << "[" << t.op_id << ",[" << t.target_ids.front();

        for (size_t i = 1; i != t.target_ids.size(); ++i) {
            os << "," << t.target_ids[i];
        }

        return os << "]]";
    }
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
