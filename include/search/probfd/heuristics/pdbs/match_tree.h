#ifndef PROBFD_PDBS_MATCH_TREE_H
#define PROBFD_PDBS_MATCH_TREE_H

#include "probfd/heuristics/pdbs/state_rank.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionOperator;
class StateRankingFunction;

/**
 * @brief Applicable actions generator for projections.
 */
class MatchTree {
    struct Node;

    std::unique_ptr<Node> root;
    std::vector<ProjectionOperator> projection_operators;

    void insert_recursive(
        const VariablesProxy& task_variables,
        const StateRankingFunction& ranking_function,
        size_t op_index,
        const std::vector<FactPair>& progression_preconditions,
        int pre_index,
        std::unique_ptr<Node>& edge_from_parent);

    void get_applicable_operators_recursive(
        Node* node,
        StateRank abstract_state,
        std::vector<const ProjectionOperator*>& operators) const;

    void dump_recursive(std::ostream& out, Node* node) const;

public:
    MatchTree(size_t hint_num_operators = 0);

    ~MatchTree();

    /**
     * @brief Insert a new projection operator with the given preconditions
     * into the match tree.
     */
    void insert(
        const VariablesProxy& task_variables,
        const StateRankingFunction& ranking_function,
        ProjectionOperator op,
        const std::vector<FactPair>& progression_preconditions);

    /**
     * @brief Obtain the applicable prohjection operators for a given abstract
     * state.
     */
    void get_applicable_operators(
        StateRank abstract_state,
        std::vector<const ProjectionOperator*>& operators) const;

    /**
     * @brief Obtain an action ID for a projection operator.
     */
    ActionID get_operator_index(const ProjectionOperator& op) const;

    /**
     * @brief Obtain the projection operator at a specific index.
     */
    const ProjectionOperator& get_index_operator(int index) const;

    /**
     * @brief Dump the match tree to an output stream.
     */
    void dump(std::ostream& out) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif
