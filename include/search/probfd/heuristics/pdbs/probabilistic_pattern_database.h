#ifndef PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PATTERN_DATABASE_H
#define PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PATTERN_DATABASE_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/abstract_policy.h"
#include "probfd/heuristics/pdbs/engine_interfaces.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <ostream>
#include <set>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

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
class ProbabilisticPatternDatabase {
    StateRankingFunction ranking_function_;
    std::vector<value_t> value_table;
    const value_t dead_end_cost;

    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        Pattern pattern,
        value_t dead_end_cost);

    ProbabilisticPatternDatabase(
        StateRankingFunction ranking_function,
        value_t dead_end_cost);

    void compute_value_table(
        ProjectionStateSpace& state_space,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        const StateRankEvaluator& heuristic,
        double max_time);

public:
    /**
     * @brief Construct a probability-aware pattern database for a given task
     * and pattern.
     *
     * @param task_proxy The input task.
     * @param pattern The pattern inducing the projection.
     * @param task_cost_function The task's cost function.
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
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        Pattern pattern,
        TaskCostFunction& task_cost_function,
        const State& initial_state,
        bool operator_pruning = true,
        const StateRankEvaluator& heuristic =
            ConstantEvaluator<StateRank>(0_vt),
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct the probability-aware pattern database for the given
     * projection state space.
     *
     * @param projection The projection state space.
     * @param ranking_function The state ranking function for the projection.
     * @param projection_cost_function The cost function of the projection.
     * @param initial_state The abstract initial state of the projection. States
     * unreachable from the initial state are treated as dead ends.
     * @param heuristic An admissible heuristic for the projection, used to
     * accelerate the computation of the lookup table.
     * @param max_time The time limit for construction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilisticPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        AbstractCostFunction& projection_cost_function,
        StateRank initial_state,
        const StateRankEvaluator& heuristic =
            ConstantEvaluator<StateRank>(0_vt),
        double max_time = std::numeric_limits<double>::infinity());

    /**
     * @brief Construct a probability-aware pattern database for a given task
     * from a determinization-based pattern database, used as a heuristic.
     *
     * @param task_proxy The input task.
     * @param pdb The determinization-based pattern database. This PDB must be
     * constructed for the same pattern.
     * @param task_cost_function The task's cost function.
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
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const ::pdbs::PatternDatabase& pdb,
        TaskCostFunction& task_cost_function,
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
     * @param projection_cost_function The cost function of the projection.
     * @param initial_state The abstract initial state of the projection. States
     * unreachable from the initial state are treated as dead ends.
     * @param pdb The determinization-based pattern database. This PDB must be
     * constructed for the same pattern.
     * @param max_time The time limit for construction. If exceeded, a
     * utils::TimeoutException will be thrown.
     *
     * @throws utils::TimeoutException if the given \p max_time is exceeded.
     */
    ProbabilisticPatternDatabase(
        ProjectionStateSpace& projection,
        StateRankingFunction ranking_function,
        AbstractCostFunction& projection_cost_function,
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
     * @param pdb A previous probability-aware pattern database.
     * @param add_var A task variable with with the previous PDB shall be
     * extended.
     * @param task_cost_function The task's cost function.
     * @param initial_state The initial state for the exhaustive solver.
     * States unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     */
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const ProbabilisticPatternDatabase& pdb,
        int add_var,
        TaskCostFunction& task_cost_function,
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
     * @param projection_cost_function The cost function of the projection.
     * @param initial_state The rank of the initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     * @param pdb A previous probability-aware pattern database.
     * @param add_var A task variable with with the previous PDB shall be
     * extended.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     */
    ProbabilisticPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        AbstractCostFunction& projection_cost_function,
        StateRank initial_state,
        const ProbabilisticPatternDatabase& pdb,
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
     * @param left A previous probability-aware pattern database for the given
     * task.
     * @param right A previous probability-aware pattern database for the given
     * task.
     * @param task_cost_function The task's cost function.
     * @param initial_state The initial state for the exhaustive solver. States
     * unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     */
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const ProbabilisticPatternDatabase& left,
        const ProbabilisticPatternDatabase& right,
        TaskCostFunction& task_cost_function,
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
     * @param projection_cost_function The cost function of the projection.
     * @param initial_state The rank of the initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     * @param left A previous pattern database for the given task.
     * @param right A previous pattern database for the given task.
     * @param max_time The time limit for contruction. If exceeded, a
     * utils::TimeoutException will be thrown.
     */
    ProbabilisticPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        AbstractCostFunction& projection_cost_function,
        StateRank initial_state,
        const ProbabilisticPatternDatabase& left,
        const ProbabilisticPatternDatabase& right,
        double max_time = std::numeric_limits<double>::infinity());

    /// Get the pattern of the pattern database.
    [[nodiscard]] const Pattern& get_pattern() const;

    /// Get the number of states in this PDB's projection.
    [[nodiscard]] unsigned int num_states() const;

    /// Check if the corresponding abstract state in the PDB's projection of an
    /// input state is a dead end.
    [[nodiscard]] bool is_dead_end(const State& s) const;

    /// Check if the abstract state in the PDB's projection corresponging to an
    /// input state rank is a dead end.
    [[nodiscard]] bool is_dead_end(StateRank rank) const;

    /// Compute the state rank of the abstract state of an input state.
    [[nodiscard]] StateRank get_abstract_state(const State& state) const;

    /// Get the optimal state value of the abstract state corresponding to the
    /// input state.
    [[nodiscard]] value_t lookup_estimate(const State& s) const;

    /// Look up the estimate of an abstract state specified by state rank in
    /// the lookup table.
    [[nodiscard]] value_t lookup_estimate(StateRank s) const;

    /// Query a heuristic evaluation for an input state.
    [[nodiscard]] EvaluationResult evaluate(const State& s) const;

    /// Query a heuristic evaluation for an abstract state.
    [[nodiscard]] EvaluationResult evaluate(StateRank s) const;

    /**
     * @brief Extract an abstract optimal policy for the PDB's projection from
     * the PDB value table.
     *
     * Tie-breaking is performed randomly using the input RNG. If the \p
     * wildcard option is specified, a wildcard policy will be returned, i.e., a
     * policy that assigns multiple equivalent operators to a abstract state.
     */
    [[nodiscard]] std::unique_ptr<AbstractPolicy>
    compute_optimal_abstract_policy(
        ProjectionStateSpace& state_space,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard) const;

    /**
     * @brief Extracts a greedy policy for the PDB's projection from the PDB
     * value table, which may not be optimal if traps are existent.
     *
     * Tie-breaking is performed randomly using the input RNG. If the \p
     * wildcard option is specified, a wildcard policy will be returned, i.e., a
     * policy that assigns multiple equivalent operators to a abstract state.
     */
    [[nodiscard]] std::unique_ptr<AbstractPolicy>
    compute_greedy_abstract_policy(
        ProjectionStateSpace& state_space,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard) const;

    void compute_saturated_costs(
        ProjectionStateSpace& state_space,
        std::vector<value_t>& saturated_costs) const;

    /**
     * @brief Dump the PDB's projection as a dot graph to a specified path with
     * or without transition labels shown.
     */
    void dump_graphviz(
        const ProbabilisticTaskProxy& task_proxy,
        ProjectionStateSpace& state_space,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        std::ostream& out,
        bool transition_labels) const;

#if !defined(NDEBUG) && defined(USE_LP)
private:
    void verify(
        ProjectionStateSpace& state_space,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        const std::vector<StateID>& proper_states);
#endif
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PROJECTION_H__