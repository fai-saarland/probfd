#ifndef PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H
#define PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/engine_interfaces.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include <vector>

namespace probfd {
class ProbabilisticTaskProxy;

namespace heuristics {
namespace pdbs {

class StateRankingFunction;

/// Represents the state space of a projection of a probabilistic planning task.
class ProjectionStateSpace {
    // For now, only the PDB implementations need this class.
    friend ProbabilisticPatternDatabase;
    friend MaxProbPatternDatabase;
    friend SSPPatternDatabase;

    MatchTree match_tree_;

    engine_interfaces::StateIDMap<StateRank> state_id_map;
    engine_interfaces::ActionIDMap<const AbstractOperator*> action_id_map;
    engine_interfaces::TransitionGenerator<const AbstractOperator*>
        transition_gen;

    StateRank initial_state_;

public:
    /// \todo Hacked access...
    std::vector<bool> goal_state_flags_;

    ProjectionStateSpace(
        const ProbabilisticTaskProxy& task_proxy,
        const StateRankingFunction& ranking_function,
        bool operator_pruning = true);
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif