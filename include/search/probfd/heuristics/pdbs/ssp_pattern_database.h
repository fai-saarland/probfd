#ifndef PROBFD_HEURISTICS_PDBS_EXPCOST_PROJECTION_H
#define PROBFD_HEURISTICS_PDBS_EXPCOST_PROJECTION_H

#include "probfd/heuristics/pdbs/abstract_policy.h"
#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

#include "probfd/heuristics/constant_evaluator.h"

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

/// Implements a probabilistic pattern database for SSPs.
class SSPPatternDatabase : public ProbabilisticPatternDatabase {
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
    SSPPatternDatabase(
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
    SSPPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        ProjectionCostFunction& cost_function,
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
    SSPPatternDatabase(
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
    SSPPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        ProjectionCostFunction& cost_function,
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
    SSPPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const SSPPatternDatabase& pdb,
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
    SSPPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        ProjectionCostFunction& cost_function,
        StateRank initial_state,
        const SSPPatternDatabase& pdb,
        int add_var);

    /**
     * @brief Constructs a pattern database for the union of the two patterns
     * of two previous PDBs.
     *
     * This constructor makes use of the supplied PDBs by using the minimum
     * over their induced heuristics to accelerate the value table computation.
     *
     * \todo One could do even better by exploiting potential additivity of the
     * two patterns.
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
    SSPPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const SSPPatternDatabase& left,
        const SSPPatternDatabase& right,
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
     * \todo One could do even better by exploiting potential additivity of the
     * two patterns.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param left A previous pattern database for the given task.
     * @param right A previous pattern database for the given task.
     * @param initial_state The rank of the initial state for the exhaustive
     * solver. States unreachable from this state are treated as dead ends.
     */
    SSPPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        ProjectionCostFunction& cost_function,
        StateRank initial_state,
        const SSPPatternDatabase& left,
        const SSPPatternDatabase& right);

    /// Get a heuristic evaluation for an input state.
    [[nodiscard]] EvaluationResult evaluate(const State& s) const;

    /// Get a heuristic evaluation for an abstract state given by a state rank.
    [[nodiscard]] EvaluationResult evaluate(StateRank s) const;

private:
    void compute_value_table(
        ProjectionStateSpace& state_space,
        ProjectionCostFunction& cost_function,
        StateRank initial_state,
        const StateRankEvaluator& heuristic);

#if !defined(NDEBUG) && defined(USE_LP)
    void verify(
        ProjectionStateSpace& state_space,
        ProjectionCostFunction& cost_function,
        StateRank initial_state,
        const std::vector<StateID>& proper_states);
#endif
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __EXPCOST_PROJECTION_H__