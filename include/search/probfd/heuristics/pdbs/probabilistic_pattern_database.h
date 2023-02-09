#ifndef PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PATTERN_DATABASE_H
#define PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PATTERN_DATABASE_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/abstract_policy.h"
#include "probfd/heuristics/pdbs/engine_interfaces.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

/**
 * @brief Base class for the probabilistic pattern database implementations for
 * SSPs and MaxProb.
 *
 * This class currently bundles four objects:
 * - The state ranking function converting states to state ranks.
 * - The lookup table containing the optimal state value for every state,
 * accessed by state rank.
 * - A lookup table of flags specifying whether a specific abstract state is
 * a dead end in the projection, accessed by state rank.
 * - The state space of the projection.
 *
 * \todo The state space should be stored seperately since it can be free'd
 * much earlier than the PDB. Also, is the dead end information really relevant?
 * We probably only need to know whether a state is unsolvable, and that could
 * be deduced from the state estimate already...
 */
class ProbabilisticPatternDatabase {
    struct StateRankSpace {
        StateRank initial_state_;
        std::vector<AbstractOperator> abstract_operators_;
        MatchTree match_tree_;
        std::vector<bool> goal_state_flags_;

        StateRankSpace(
            const ProbabilisticTaskProxy& task_proxy,
            const StateRankingFunction& mapper,
            bool operator_pruning = true);

        void setup_abstract_goal(
            const ProbabilisticTaskProxy& task_proxy,
            const StateRankingFunction& mapper);
        bool is_goal(StateRank s) const;
    };

protected:
    StateRankingFunction state_mapper_;
    StateRankSpace abstract_state_space_;
    std::vector<StateID> dead_ends_;
    std::vector<value_t> value_table;

protected:
    ProbabilisticPatternDatabase(
        const ProbabilisticTaskProxy& task_proxy,
        Pattern pattern,
        bool operator_pruning,
        value_t fill);

public:
    /// Get the state ranking function of the PDB.
    const StateRankingFunction& get_abstract_state_mapper() const;

    /// Get the number of states in this PDB's projection.
    unsigned int num_states() const;

    /// Check if the corresponding abstract state in the PDB's projection of an
    /// input state is a dead end.
    bool is_dead_end(const State& s) const;

    /// Check if the abstract state in the PDB's projection corresponging to an
    /// input state rank is a dead end.
    bool is_dead_end(StateRank rank) const;

    /// Check if the abstract state in the PDB's projection corresponging to an
    /// input state rank is a goal state.
    bool is_goal(StateRank rank) const;

    /// Compute the state rank of the abstract state of an input state.
    StateRank get_abstract_state(const State& state) const;

    /// Compute the state rank of the abstract state of an input state.
    StateRank get_abstract_state(const std::vector<int>& s) const;

    /// Get the optimal state value of the abstract state corresponding to the
    /// input state.
    [[nodiscard]] value_t lookup(const State& s) const;

    /// Get the optimal state value of the abstract state corresponding to the
    /// input state rank.
    [[nodiscard]] value_t lookup(StateRank s) const;

    // Get the pattern of the pattern database.
    const Pattern& get_pattern() const;

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
    AbstractPolicy get_optimal_abstract_policy(
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
    AbstractPolicy get_optimal_abstract_policy_no_traps(
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard = false) const;

protected:
    void dump_graphviz(
        const std::string& path,
        std::function<std::string(const StateRank&)> sts,
        AbstractCostFunction& costs,
        bool transition_labels) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PROJECTION_H__