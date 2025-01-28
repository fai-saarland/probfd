#ifndef PROBFD_PDBS_PROBABILITY_AWARE_PATTERN_DATABASE_H
#define PROBFD_PDBS_PROBABILITY_AWARE_PATTERN_DATABASE_H

#include "probfd/pdbs/evaluators.h"
#include "probfd/pdbs/state_ranking_function.h"
#include "probfd/pdbs/types.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/fdr_types.h"

#include <limits>
#include <vector>

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs {
class ProjectionStateSpace;
}

namespace probfd::pdbs {

/**
 * @brief Implementation of a probability-aware pattern database.
 *
 * This class bundles two objects:
 * - A state ranking function, which implements the state mapping of the
 * projection. The ranking function also includes the pattern of the projection.
 * - A lookup table containing the optimal state values of all abstract states.
 */
struct ProbabilityAwarePatternDatabase {
    /// The state ranking function.
    StateRankingFunction ranking_function;

    /// The abstract state distances ordered by abstract state rank.
    std::vector<value_t> value_table;

    /**
     * @brief Constructs the ranking function of the pattern database for the
     * given variable space and pattern.
     *
     * The distance table is allocated such that it has size equal to the total
     * number of abstract states and is filled with NaNs.
     */
    ProbabilityAwarePatternDatabase(
        const VariablesProxy& variables,
        Pattern pattern);

    /// Get the pattern of the pattern database.
    [[nodiscard]]
    const Pattern& get_pattern() const;

    /// Get the number of states in this PDB's projection.
    [[nodiscard]]
    unsigned int num_states() const;

    /// Compute the state rank of the abstract state of an input state.
    [[nodiscard]]
    StateRank get_abstract_state(const State& state) const;

    /// Get the optimal state value of the abstract state corresponding to the
    /// input state.
    [[nodiscard]]
    value_t lookup_estimate(const State& s) const;

    /// Look up the estimate of an abstract state specified by state rank in
    /// the lookup table.
    [[nodiscard]]
    value_t lookup_estimate(StateRank s) const;
};

/**
 * @brief Computes the goal distances for reachable states of a pattern
 * database.
 *
 * @param pdb The pattern database to populate the goal distances for.
 * @param mdp The state space of the projection induced by the pattern database.
 * @param abstract_initial_state Initial state that specifies the reachability
 * of each state. Goal distances of abstract states unreachable from this state
 * are left untouched.
 * @param heuristic An optional heuristic to accelerate the computation.
 * @param max_time Maximum time allowed for the computation. If the time is
 * exceeded, throws utils::TimeoutException. Time management is not exact.
 */
void compute_distances(
    ProbabilityAwarePatternDatabase& pdb,
    ProjectionStateSpace& mdp,
    StateRank abstract_initial_state,
    const Evaluator<StateRank>& heuristic =
        heuristics::BlindEvaluator<StateRank>(),
    double max_time = std::numeric_limits<double>::infinity());

/**
 * @brief Computes the goal distances for all reachable abstract states of a
 * pattern database.
 *
 * @param pdb The pattern database to populate the goal distances for.
 * @param task_proxy The planning task on which the abstraction is applied.
 * @param task_cost_function The cost function with respect to which goal
 * distances are computed.
 * @param abstract_initial_state Initial state that specifies the reachability
 * of each state. Goal distances of abstract states unreachable from this state
 * are left untouched.
 * @param heuristic An optional heuristic to accelerate the computation.
 * @param max_time Maximum time allowed for the computation. If the time is
 * exceeded, throws utils::TimeoutException. Time management is not exact.
 */
void compute_distances(
    ProbabilityAwarePatternDatabase& pdb,
    const ProbabilisticTaskProxy& task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    StateRank abstract_initial_state,
    const Evaluator<StateRank>& heuristic =
        heuristics::BlindEvaluator<StateRank>(),
    bool operator_pruning = true,
    double max_time = std::numeric_limits<double>::infinity());

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROBABILITY_AWARE_PATTERN_DATABASE_H
