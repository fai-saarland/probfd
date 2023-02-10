#ifndef PROBFD_HEURISTICS_PDBS_MAXPROB_PATTERN_DATABASE_H
#define PROBFD_HEURISTICS_PDBS_MAXPROB_PATTERN_DATABASE_H

#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

#include "probfd/heuristics/constant_evaluator.h"

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

/// Implements a probabilistic pattern database for MaxProb.
class MaxProbPatternDatabase : public ProbabilisticPatternDatabase {
public:
    /**
     * @brief Constructs a pattern database from a given task and pattern, using
     * the specified construction options.
     *
     * @param task_proxy The input task with respect to which the projection is
     * constructed.
     * @param pattern The pattern of the pattern database.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection state space.
     * @param heuristic A heuristic used to accelerate the computation of the
     * value table.
     */
    MaxProbPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const Pattern& pattern,
        bool operator_pruning = true,
        const StateRankEvaluator& heuristic =
            ConstantEvaluator<StateRank>(0_vt));

    /**
     * @brief Constructs a pattern database from a preconstructed projection
     * state space and ranking function.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param pattern The preconstructed state ranking function for the PDB.
     * @param heuristic A heuristic used to accelerate the computation of the
     * value table.
     */
    MaxProbPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
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
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     */
    MaxProbPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const ::pdbs::PatternDatabase& pdb,
        bool operator_pruning = true);

    /**
     * @brief Constructs a pattern database from a preconstructed projection
     * state space and ranking function using a deterministic PDB with the same
     * pattern as a heuristic provider.
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param pdb A deterministic pattern database for the same pattern.
     */
    MaxProbPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
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
     * @param add_var The additional variable to consider for this PDB.
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     */
    MaxProbPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const MaxProbPatternDatabase& pdb,
        int add_var,
        bool operator_pruning = true);

    /**
     * @brief Constructs a pattern database for the pattern of a previous
     * pattern database with one additional variable.
     *
     * This constructor makes use of the supplied PDB by using its
     * induced heuristic to accelerate the value table computation.
     *
     *
     * @param state_space The preconstructed state space of the projection.
     * @param ranking_function The preconstructed ranking function for the PDB.
     * @param pdb A previous pattern database for the given task.
     * @param add_var The additional variable to consider for this PDB.
     */
    MaxProbPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        const MaxProbPatternDatabase& pdb,
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
     * @param operator_pruning Whether equivalent operators shall be pruned
     * during construction of the projection.
     */
    MaxProbPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        const MaxProbPatternDatabase& left,
        const MaxProbPatternDatabase& right,
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
     */
    MaxProbPatternDatabase(
        ProjectionStateSpace& state_space,
        StateRankingFunction ranking_function,
        const MaxProbPatternDatabase& left,
        const MaxProbPatternDatabase& right);

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
     *
     * \todo Use flag to specify whether traps can be assumed absent...
     */
    std::unique_ptr<AbstractPolicy> get_optimal_abstract_policy(
        ProjectionStateSpace& state_space,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard = false) const;

    /**
     * @brief Extracts an abstract optimal policy for the PDB's projection from
     * the PDB value table, assuming traps are absent.
     *
     * Tie-breaking is performed randomly using the input RNG. If the \p
     * wildcard option is specified, a wildcard policy will be returned, i.e., a
     * policy that assigns multiple equivalent operators to a abstract state.
     *
     * \todo Use flag to specify whether traps can be assumed absent...
     */
    std::unique_ptr<AbstractPolicy> get_optimal_abstract_policy_no_traps(
        ProjectionStateSpace& state_space,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard = false) const;

    /// Dump the PDB's projection as a dot graph to a specified path with or
    /// without transition labels shown.
    void dump_graphviz(
        ProjectionStateSpace& state_space,
        const std::string& path,
        bool transition_labels = true);

private:
    void compute_value_table(
        ProjectionStateSpace& state_space,
        const StateRankEvaluator& heuristic);

#if !defined(NDEBUG) && defined(USE_LP)
    void verify(ProjectionStateSpace& state_space);
#endif
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __MAXPROB_PROJECTION_H__