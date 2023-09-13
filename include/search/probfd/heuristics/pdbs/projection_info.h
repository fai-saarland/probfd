#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_PROJECTION_INFO_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_PROJECTION_INFO_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include <limits>
#include <memory>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

class StateRankingFunction;
class ProjectionStateSpace;
class ProbabilityAwarePatternDatabase;

struct ProjectionInfo {
    std::unique_ptr<ProjectionStateSpace> mdp;
    std::unique_ptr<ProbabilityAwarePatternDatabase> pdb;

    ProjectionInfo();

    ProjectionInfo(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const AbstractionEvaluator& heuristic,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    ProjectionInfo(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const ProbabilityAwarePatternDatabase& pdb,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    ProjectionInfo(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const ProbabilityAwarePatternDatabase& left,
        const ProbabilityAwarePatternDatabase& right,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    ProjectionInfo(ProjectionInfo&&) noexcept;
    ProjectionInfo& operator=(ProjectionInfo&&) noexcept;

    ~ProjectionInfo();

    /// Get the pattern of the projection.
    [[nodiscard]] const Pattern& get_pattern() const;

    /// Get the abstraction mapping of the projection.
    [[nodiscard]] const StateRankingFunction&
    get_state_ranking_function() const;

    /// Get the abstraction mapping of the projection.
    [[nodiscard]] const std::vector<value_t>& get_value_table() const;

    /// Get the number of states in this projection.
    [[nodiscard]] unsigned int num_states() const;

    /// Compute the abstract state index for an input state.
    [[nodiscard]] AbstractStateIndex
    get_abstract_state(const State& state) const;

    /// Get the optimal state value of the abstract state corresponding to the
    /// input state.
    [[nodiscard]] value_t lookup_estimate(const State& s) const;

    /// Look up the estimate of an abstract state specified by index in
    /// the lookup table.
    [[nodiscard]] value_t lookup_estimate(AbstractStateIndex s) const;

    /// Check if the abstract state with given index is an abstract goal.
    [[nodiscard]] bool is_goal(AbstractStateIndex s) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_PROJECTION_INFO_H
