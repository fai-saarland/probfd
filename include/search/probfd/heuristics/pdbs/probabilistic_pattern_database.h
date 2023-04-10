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
#include <memory>
#include <ostream>
#include <set>
#include <vector>


class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

/**
 * @brief Implementation of a probabilistic pattern database.
 *
 * This class bundles two objects:
 * - The state ranking function of the projection, converting task states to
 * ranks of the projection. The ranking function also includes the pattern
 * considered by the projection.
 * - The lookup table containing the optimal state value for every state of the
 * projection.
 *
 * A PDB does not store information about the state space of the projection for
 * which it was constructed. If it needs to be maintained, it should be
 * pre-computed and stored seperately. The PDB can then be constructed from an
 * input state space directly without creating its own temporary instantiation
 * by using a suitable constructor.
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
        const StateRankEvaluator& heuristic);

public:
    /**
     * @brief Constructs a pattern database from a given task and pattern, using
     * the specified construction options.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param pattern The pattern of the pattern database.
     * @param initial_state The initial state for the exhaustive solver.
     * States unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     * @param heuristic A heuristic used to accelerate the computation of the
     * value table.
     */
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        Pattern pattern,
        TaskCostFunction& task_cost_function,
        const State& initial_state,
        bool operator_pruning = true,
        const StateRankEvaluator& heuristic =
            ConstantEvaluator<StateRank>(0_vt));

    /**
     * @brief Constructs a pattern database from a given task and pattern, using
     * the specified construction options.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param initial_state The rank of the initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     * @param heuristic A heuristic used to accelerate the computation of the
     * value table.
     */
    ProbabilisticPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        const StateRankEvaluator& heuristic =
            ConstantEvaluator<StateRank>(0_vt));

    /**
     * @brief Constructs a pattern database from a given task and the pattern of
     * a given deterministic PDB, using the specified construction options.
     *
     * This constructor makes use of the supplied deterministic PDB by using its
     * induced heuristic to accelerate the value table computation.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param pdb A previous deterministic pattern database for the all-outcomes
     * determinization of the given task.
     * @param initial_state The initial state for the exhaustive solver.
     * States unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     */
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const ::pdbs::PatternDatabase& pdb,
        TaskCostFunction& task_cost_function,
        const State& initial_state,
        bool operator_pruning = true);

    /**
     * @brief Constructs a pattern database from a given task and the pattern of
     * a given deterministic PDB, using the specified construction options.
     *
     * This constructor makes use of the supplied deterministic PDB by using its
     * induced heuristic to accelerate the value table computation.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param initial_state The rank of the  initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     */
    ProbabilisticPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        const ::pdbs::PatternDatabase& pdb);

    /**
     * @brief Constructs a pattern database for the pattern of a previous
     * pattern database with one additional variable.
     *
     * This constructor makes use of the supplied PDB by using its
     * induced heuristic to accelerate the value table computation.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param pdb A previous pattern database for the given task.
     * @param initial_state The initial state for the exhaustive solver.
     * States unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     */
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const ProbabilisticPatternDatabase& pdb,
        int add_var,
        TaskCostFunction& task_cost_function,
        const State& initial_state,
        bool operator_pruning = true);

    /**
     * @brief Constructs a pattern database for the pattern of a previous
     * pattern database with one additional variable.
     *
     * This constructor makes use of the supplied PDB by using its
     * induced heuristic to accelerate the value table computation.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param initial_state The rank of the initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     */
    ProbabilisticPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        const ProbabilisticPatternDatabase& pdb,
        int add_var);

    /**
     * @brief Constructs a pattern database for the union of the two patterns
     * of two previous PDBs.
     *
     * This constructor makes use of the supplied PDBs by using the minimum
     * over their induced heuristics to accelerate the value table computation.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param left A previous pattern database for the given task.
     * @param right A previous pattern database for the given task.
     * @param initial_state The initial state for the exhaustive solver. States
     * unreachable from this state are treated as dead ends.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     */
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const ProbabilisticPatternDatabase& left,
        const ProbabilisticPatternDatabase& right,
        TaskCostFunction& task_cost_function,
        const State& initial_state,
        bool operator_pruning = true);

    /**
     * @brief Constructs a pattern database for the union of the two patterns
     * of two previous PDBs.
     *
     * This constructor makes use of the supplied PDBs by using the minimum
     * over their induced heuristics to accelerate the value table computation.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param left A previous pattern database for the given task.
     * @param right A previous pattern database for the given task.
     * @param initial_state The rank of the initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     */
    ProbabilisticPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        AbstractCostFunction& cost_function,
        StateRank initial_state,
        const ProbabilisticPatternDatabase& left,
        const ProbabilisticPatternDatabase& right);

    // Get the pattern of the pattern database.
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
    [[nodiscard]] value_t lookup(const State& s) const;

    /// Get the optimal state value of the abstract state corresponding to the
    /// input state rank.
    [[nodiscard]] value_t lookup(StateRank s) const;

    /// Get a heuristic evaluation for an input state.
    [[nodiscard]] EvaluationResult evaluate(const State& s) const;

    /// Get a heuristic evaluation for an abstract state given by a state rank.
    [[nodiscard]] EvaluationResult evaluate(StateRank s) const;

    /**
     * @brief Extracts an abstract optimal policy for the PDB's projection from
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

    /// Dump the PDB's projection as a dot graph to a specified path with or
    /// without transition labels shown.
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