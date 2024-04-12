#ifndef PROBFD_PDBS_MATCH_TREE_H
#define PROBFD_PDBS_MATCH_TREE_H

#include "probfd/pdbs/types.h"

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <stack>
#include <vector>

// Forward Declarations
struct FactPair;

namespace probfd {
template <typename>
struct Transition;
}

namespace probfd::pdbs {
class AssignmentEnumerator;
class ProjectionOperator;
class ProjectionStateSpace;
} // namespace probfd::pdbs

namespace probfd::pdbs {

/**
 * @brief Applicable actions generator for projections.
 */
class MatchTree {
    struct Node;

    std::unique_ptr<Node> root_;
    std::vector<ProjectionOperator> projection_operators_;
    mutable std::stack<Node*> nodes_; // reuse storage

public:
    explicit MatchTree(size_t hint_num_operators = 0);

    ~MatchTree();

    /**
     * @brief Insert a new projection operator with the given preconditions
     * into the match tree.
     */
    void insert(
        const AssignmentEnumerator& ranking_function,
        ProjectionOperator op,
        const std::vector<FactPair>& progression_preconditions,
        bool operator_pruning);

    /**
     * @brief Obtain the applicable prohjection operators for a given abstract
     * state.
     */
    void get_applicable_operators(
        StateRank abstract_state,
        std::vector<const ProjectionOperator*>& operators) const;

    /**
     * @brief Obtain the applicable prohjection operators for a given
     * abstract state.
     */
    void generate_all_transitions(
        StateRank abstract_state,
        std::vector<Transition<const ProjectionOperator*>>& transitions,
        ProjectionStateSpace& state_space) const;

    /**
     * @brief Dump the match tree to an output stream.
     */
    void dump(std::ostream& out) const;

private:
    void dump_recursive(std::ostream& out, Node* node) const;
};

} // namespace probfd::pdbs

#endif
