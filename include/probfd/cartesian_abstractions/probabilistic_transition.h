#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_PROBABILISTIC_TRANSITION_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_PROBABILISTIC_TRANSITION_H

#include <cassert>
#include <compare>
#include <iostream>
#include <vector>

namespace probfd::cartesian_abstractions {

struct ProbabilisticTransition {
    int source_id;
    int op_id;
    std::vector<int> target_ids; // One target for each operator effect

    ProbabilisticTransition(
        int source_id,
        int op_id,
        std::vector<int> target_ids)
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

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_PROBABILISTIC_TRANSITION_H
