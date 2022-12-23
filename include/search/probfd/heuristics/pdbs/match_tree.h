#ifndef PROBFD_PDBS_MATCH_TREE_H
#define PROBFD_PDBS_MATCH_TREE_H

#include "probfd/heuristics/pdbs/types.h"

#include <cstddef>
#include <vector>

namespace probfd {
namespace heuristics {
namespace pdbs {

struct AbstractState;
struct AbstractOperator;
class AbstractStateMapper;

/*
 * Successor Generator for abstract operators.
 */
class MatchTree {
    struct Node;

    // See PatternDatabase for documentation on pattern and hash_multipliers.
    Pattern pattern;
    const AbstractStateMapper& mapper;
    Node* root;

    // Pointer to first operator
    const AbstractOperator* first;

    void insert_recursive(
        size_t op_index,
        const std::vector<std::pair<int, int>>& progression_preconditions,
        int pre_index,
        Node** edge_from_parent);

    void get_applicable_operators_recursive(
        Node* node,
        AbstractState abstract_state,
        std::vector<const AbstractOperator*>& operators) const;

    void dump_recursive(Node* node) const;

public:
    // Initialize an empty match tree.
    MatchTree(const Pattern& pattern, const AbstractStateMapper& mapper);

    ~MatchTree();

    /* Insert an abstract operator into the match tree, creating or
       enlarging it. */
    void insert(
        size_t op_index,
        const std::vector<std::pair<int, int>>& progression_preconditions);

    /*
      Extracts all IDs of applicable abstract operators for the abstract state
      given by state_index (the index is converted back to variable/values
      pairs).
    */
    void get_applicable_operators(
        AbstractState abstract_state,
        std::vector<const AbstractOperator*>& operators) const;

    void set_first_aop(const AbstractOperator* first);

    void dump() const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif
