#ifndef PROBFD_PDBS_MATCH_TREE_H
#define PROBFD_PDBS_MATCH_TREE_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include <cstddef>
#include <vector>

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionOperator;
struct StateRank;
class StateRankingFunction;

/*
 * Successor Generator for abstract operators.
 */
class MatchTree {
    struct Node;

    Node* root = nullptr;

    std::vector<ProjectionOperator> projection_operators;

    void insert_recursive(
        const VariablesProxy& task_variables,
        const StateRankingFunction& ranking_function,
        size_t op_index,
        const std::vector<FactPair>& progression_preconditions,
        int pre_index,
        Node** edge_from_parent);

    void get_applicable_operators_recursive(
        Node* node,
        StateRank abstract_state,
        std::vector<const ProjectionOperator*>& operators) const;

    void dump_recursive(Node* node) const;

public:
    // Initialize an empty match tree.
    MatchTree(size_t hint_num_operators = 0);

    ~MatchTree();

    /* Insert an abstract operator into the match tree, creating or
       enlarging it. */
    void insert(
        const VariablesProxy& task_variables,
        const StateRankingFunction& ranking_function,
        ProjectionOperator op,
        const std::vector<FactPair>& progression_preconditions);

    /*
      Extracts all IDs of applicable abstract operators for the abstract state
      given by state_index (the index is converted back to variable/values
      pairs).
    */
    void get_applicable_operators(
        StateRank abstract_state,
        std::vector<const ProjectionOperator*>& operators) const;

    ActionID get_operator_index(const ProjectionOperator& op) const;

    const ProjectionOperator& get_index_operator(int index) const;

    void dump() const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif
