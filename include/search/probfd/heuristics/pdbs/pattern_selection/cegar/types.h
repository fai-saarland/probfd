#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_TYPES_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_TYPES_H

#include "task_proxy.h"

#include <vector>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

struct Flaw {
    bool is_goal_violation;
    int solution_index;
    int variable;

    Flaw(bool is_goal_violation, int solution_index, int variable)
        : is_goal_violation(is_goal_violation)
        , solution_index(solution_index)
        , variable(variable)
    {
    }
};

using FlawList = std::vector<Flaw>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif