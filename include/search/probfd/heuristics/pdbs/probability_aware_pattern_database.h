#ifndef PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PATTERN_DATABASE_H
#define PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PATTERN_DATABASE_H

#include "probfd/heuristics/pdbs/evaluators.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/task_proxy.h"

#include <limits>
#include <vector>

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionStateSpace;

/**
 * @brief Implementation of a probability-aware pattern database.
 *
 * Bundles two objects: The abstraction function (StateRankingFunction),
 * including the pattern of the projection, as well as the lookup table
 * containing the optimal state values of all abstract states.
 */
class ProbabilityAwarePatternDatabase {
    StateRankingFunction ranking_function_;
    std::vector<value_t> value_table;

    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        Pattern pattern,
        value_t dead_end_cost);

    ProbabilityAwarePatternDatabase(
        StateRankingFunction ranking_function,
        value_t dead_end_cost);

public:
    /**
     * @brief Construct a probability-aware pattern database for a given task
     * and pattern.
     *
     * @param task_proxy The input task.
     * @param pattern The pattern inducing the projection.
     * @param initial_state The initial state. States unreachable from the
     * initial state are treated as dead ends.
     * @param heuristic An admissible heuristic for the projection, used to
     * accelerate the computation of the lookup table.
     * @param prune_eqv_operators Specifies whether equivalent operators shall
     * be pruned during construction of the projection. This may take additional
     * time.
     * @param max_time The time limit for construction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const AbstractionEvaluator& heuristic =
            BlindEvaluator<AbstractStateIndex>(),
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task
     * and pattern.
     *
     * @param task_proxy The input task.
     * @param pattern The pattern inducing the projection.
     * @param initial_state The initial state. States unreachable from the
     * initial state are treated as dead ends.
     * @param heuristic An admissible heuristic for the projection, used to
     * accelerate the computation of the lookup table.
     * @param prune_eqv_operators Specifies whether equivalent operators shall
     * be pruned during construction of the projection. This may take additional
     * time.
     * @param max_time The time limit for construction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const AbstractionEvaluator& heuristic,
        std::unique_ptr<ProjectionStateSpace>& mdp,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task
     * from a determinization-based pattern database, used as a heuristic.
     *
     * @param task_proxy The input task.
     * @param task_cost_function The task's cost function.
     * @param pdb The determinization-based pattern database. This PDB is
     * constructed for the same pattern.
     * @param initial_state The initial state. States unreachable from the
     * initial state are treated as dead ends.
     * @param prune_eqv_operators Specifies whether equivalent operators shall
     * be pruned during construction of the projection. This may take additional
     * time.
     * @param max_time The time limit for construction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const ::pdbs::PatternDatabase& pdb,
        const State& initial_state,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task,
     * using a coarser probability-aware pattern database as a heuristic.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param task_cost_function The task's cost function.
     * @param pattern The pattern inducing the projection.
     * @param initial_state The initial state for the exhaustive solver.
     * States unreachable from this state are treated as dead ends.
     * @param pdb A coarser probability-aware pattern database.
     * @param prune_eqv_operators Specifies whether equivalent operators shall
     * be pruned during construction of the projection. This may take additional
     * time.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const ProbabilityAwarePatternDatabase& pdb,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task,
     * using a coarser probability-aware pattern database as a heuristic.
     * Additionally returns the constructed projection MDP.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param task_cost_function The task's cost function.
     * @param pattern The pattern inducing the projection.
     * @param initial_state The initial state for the exhaustive solver.
     * States unreachable from this state are treated as dead ends.
     * @param pdb A coarser probability-aware pattern database.
     * @param mdp A unique_ptr holding the constructed projection MDP after
     * construction.
     * @param prune_eqv_operators Specifies whether equivalent operators shall
     * be pruned during construction of the projection. This may take additional
     * time.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const ProbabilityAwarePatternDatabase& pdb,
        std::unique_ptr<ProjectionStateSpace>& mdp,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task,
     * using the maximum over two coarser probability-aware pattern databases
     * as a heuristic.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param task_cost_function The task's cost function.
     * @param pattern The pattern inducing the projection.
     * @param initial_state The initial state for the exhaustive solver. States
     * unreachable from this state are treated as dead ends.
     * @param left A previous probability-aware pattern database for the given
     * task.
     * @param right A previous probability-aware pattern database for the given
     * task.
     * @param prune_eqv_operators Specifies whether equivalent operators shall
     * be pruned during construction of the projection. This may take additional
     * time.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const ProbabilityAwarePatternDatabase& left,
        const ProbabilityAwarePatternDatabase& right,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task,
     * using the maximum over two coarser probability-aware pattern databases
     * as a heuristic. Additionally returns the constructed projection MDP.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param task_cost_function The task's cost function.
     * @param pattern The pattern inducing the projection.
     * @param initial_state The initial state for the exhaustive solver. States
     * unreachable from this state are treated as dead ends.
     * @param left A coarser probability-aware pattern database for the given
     * task.
     * @param right A coarser probability-aware pattern database for the given
     * task.
     * @param mdp A unique_ptr holding the constructed projection MDP after
     * construction.
     * @param prune_eqv_operators Specifies whether equivalent operators shall
     * be pruned during construction of the projection. This may take additional
     * time.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        const State& initial_state,
        const ProbabilityAwarePatternDatabase& left,
        const ProbabilityAwarePatternDatabase& right,
        std::unique_ptr<ProjectionStateSpace>& mdp,
        bool prune_eqv_operators = true,
        double max_time = std::numeric_limits<double>::infinity());

    /// Get the pattern of the pattern database.
    [[nodiscard]] const Pattern& get_pattern() const;

    /// Get the abstraction mapping of the pattern database.
    [[nodiscard]] const StateRankingFunction&
    get_state_ranking_function() const;

    /// Get the abstraction mapping of the pattern database.
    [[nodiscard]] const std::vector<value_t>& get_value_table() const;

    /// Get the number of states in this PDB's projection.
    [[nodiscard]] unsigned int num_states() const;

    /// Compute the state rank of the abstract state of an input state.
    [[nodiscard]] AbstractStateIndex
    get_abstract_state(const State& state) const;

    /// Get the optimal state value of the abstract state corresponding to the
    /// input state.
    [[nodiscard]] value_t lookup_estimate(const State& s) const;

    /// Look up the estimate of an abstract state specified by state rank in
    /// the lookup table.
    [[nodiscard]] value_t lookup_estimate(AbstractStateIndex s) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PROJECTION_H__