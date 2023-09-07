#ifndef PROBFD_HEURISTICS_CARTESIAN_PROBABILISTIC_TRANSITION_H
#define PROBFD_HEURISTICS_CARTESIAN_PROBABILISTIC_TRANSITION_H

#include "probfd/heuristics/cartesian/types.h"

#include <cassert>
#include <compare>
#include <iostream>
#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

struct ProbabilisticTransition {
    AbstractStateIndex source_id;
    int op_id;

    // One target for each operator effect
    std::vector<AbstractStateIndex> target_ids; 

    ProbabilisticTransition(
        AbstractStateIndex source_id,
        int op_id,
        std::vector<AbstractStateIndex> target_ids)
        : source_id(source_id)
        , op_id(op_id)
        , target_ids(std::move(target_ids))
    {
        assert(!this->target_ids.empty());
    }

    friend std::ostream&
    operator<<(std::ostream& os, const ProbabilisticTransition& t)
    {
        os << "[" << t.source_id << "," << t.op_id << ",["
           << t.target_ids.front();

        for (size_t i = 1; i != t.target_ids.size(); ++i) {
            os << "," << t.target_ids[i];
        }

        return os << "]]";
    }
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_CARTESIAN_PROBABILISTIC_TRANSITION_H
