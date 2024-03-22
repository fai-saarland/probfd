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
 * - A state ranking function, which implements the abstraction mapping of the
 * projection. The ranking function also includes the pattern of the projection.
 * - A lookup table containing the optimal state values of all abstract states.
 *
 * A PDB does not store information about the projection state space for which
 * it was constructed. The state space should be pre-computed and stored
 * seperately if it is needed.
 */
class ProbabilityAwarePatternDatabase {
    StateRankingFunction ranking_function_;
    std::vector<value_t> value_table_;

    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        Pattern pattern);

    explicit ProbabilityAwarePatternDatabase(
        StateRankingFunction ranking_function);

public:
    /**
     * @brief Construct a probability-aware pattern database for a given task
     * and pattern.
     *
     * @param task_proxy The input task.
     * @param pattern The pattern inducing the projection.
     * @param initial_state The initial state. States unreachable from the
     * initial state are treated as dead ends.
     * @param operator_pruning Specifies whether operators with the same
     * precondition, cost, effect and effect probabilities shall be pruned
     * during construction of the projection.
     * @param heuristic An admissible heuristic for the projection, used to
     * accelerate the computation of the lookup table.
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
        bool operator_pruning = true,
        const StateRankEvaluator& heuristic =
            heuristics::BlindEvaluator<StateRank>(),
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct the probability-aware pattern database for the given
     * projection state space.
     *
     * @param projection The projection state space.
     * @param ranking_function The state ranking function for the projection.
     * @param initial_state The abstract initial state of the projection. States
     * unreachable from the initial state are treated as dead ends.
     * @param heuristic An admissible heuristic for the projection, used to
     * accelerate the computation of the lookup table.
     * @param max_time The time limit for construction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProjectionStateSpace& projection,
        StateRankingFunction ranking_function,
        StateRank initial_state,
        const StateRankEvaluator& heuristic =
            heuristics::BlindEvaluator<StateRank>(),
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task
     * from a determinization-based pattern database, used as a heuristic.
     *
     * @param task_proxy The input task.
     * @param task_cost_function The task's cost function.
     * @param pdb The determinization-based pattern database. This PDB must be
     * constructed for the same pattern.
     * @param initial_state The initial state. States unreachable from the
     * initial state are treated as dead ends.
     * @param operator_pruning Specifies whether operators with the same
     * precondition, cost, effect and effect probabilities shall be pruned
     * during construction of the projection.
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
        bool operator_pruning = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct the probability-aware pattern database for the given
     * projection state space from a determinization-based pattern database,
     * used as a heuristic.
     *
     * @param projection The projection state space.
     * @param ranking_function The state ranking function for the projection.
     * @param initial_state The abstract initial state of the projection. States
     * unreachable from the initial state are treated as dead ends.
     * @param pdb The determinization-based pattern database. This PDB must be
     * constructed for the same pattern.
     * @param max_time The time limit for construction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProjectionStateSpace& projection,
        StateRankingFunction ranking_function,
        StateRank initial_state,
        const ::pdbs::PatternDatabase& pdb,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task,
     * by extending a previous probability-aware pattern by one variable.
     *
     * This constructor makes use of the probability-aware PDB which it extends
     * by using it as a heuristic to accelerate the lookup computation.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param task_cost_function The task's cost function.
     * @param pdb A previous probability-aware pattern database.
     * @param add_var A task variable with with the previous PDB shall be
     * extended.
     * @param initial_state The initial state for the exhaustive solver.
     * States unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const ProbabilityAwarePatternDatabase& pdb,
        int add_var,
        const State& initial_state,
        bool operator_pruning = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a pattern database for the given projection state space
     * and the pattern of a previous pattern database with one additional
     * variable.
     *
     * This constructor makes use of the supplied PDB by using its
     * induced heuristic to accelerate the value table computation.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param initial_state The rank of the initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     * @param pdb A previous probability-aware pattern database.
     * @param add_var A task variable with with the previous PDB shall be
     * extended.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        StateRank initial_state,
        const ProbabilityAwarePatternDatabase& pdb,
        int add_var,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task
     * and the union of the patterns of two
     * probability-aware PDBs.
     *
     * This constructor makes use of the supplied PDBs by using the maximum
     * over their heuristics to accelerate the value table computation.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param task_cost_function The task's cost function.
     * @param left A previous probability-aware pattern database for the given
     * task.
     * @param right A previous probability-aware pattern database for the given
     * task.
     * @param initial_state The initial state for the exhaustive solver. States
     * unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const ProbabilityAwarePatternDatabase& left,
        const ProbabilityAwarePatternDatabase& right,
        const State& initial_state,
        bool operator_pruning = true,
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database the given
     * projection state space and the union of the patterns of two
     * probability-aware PDBs.
     *
     * This constructor makes use of the supplied PDBs by using the maximum
     * over their heuristics to accelerate the value table computation.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param initial_state The rank of the initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     * @param left A previous pattern database for the given task.
     * @param right A previous pattern database for the given task.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilityAwarePatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        StateRank initial_state,
        const ProbabilityAwarePatternDatabase& left,
        const ProbabilityAwarePatternDatabase& right,
        double max_time = std::numeric_limits<double>::infinity());

    /// Get the pattern of the pattern database.
    [[nodiscard]]
    const Pattern& get_pattern() const;

    /// Get the abstraction mapping of the pattern database.
    [[nodiscard]]
    const StateRankingFunction& get_state_ranking_function() const;

    /// Get the abstraction mapping of the pattern database.
    [[nodiscard]]
    const std::vector<value_t>& get_value_table() const;

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

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROBABILITY_AWARE_PATTERN_DATABASE_H
