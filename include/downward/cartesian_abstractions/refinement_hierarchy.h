#ifndef CEGAR_REFINEMENT_HIERARCHY_H
#define CEGAR_REFINEMENT_HIERARCHY_H

#include "downward/cartesian_abstractions/types.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

namespace downward {
class State;
}

namespace downward::cartesian_abstractions {
class Node;

/*
  This class stores the refinement hierarchy of a Cartesian
  abstraction. The hierarchy forms a tree with inner nodes for each
  split and leaf nodes for the abstract states.
*/
class RefinementHierarchy {
    std::vector<Node> nodes;

    NodeID add_node(int state_id);

public:
    RefinementHierarchy();

    std::pair<NodeID, NodeID> split(
        NodeID node_id,
        int var,
        const std::vector<int>& values,
        int left_state_id,
        int right_state_id);

    int get_abstract_state_id(const State& state) const;
};

class Node {
    NodeID left_child;
    NodeID right_child;

    /* Before splitting the corresponding state for var and value, both
       members hold UNDEFINED. */
    int var;
    std::vector<int> values;

    // When splitting the corresponding state, we change this value to
    // UNDEFINED.
    int state_id;

    bool information_is_valid() const;

public:
    explicit Node(int state_id);

    bool is_split() const;

    void split(
        int var,
        std::vector<int> values,
        NodeID left_child,
        NodeID right_child);

    int get_var() const
    {
        assert(is_split());
        return var;
    }

    NodeID get_child(int value) const
    {
        assert(is_split());
        return std::ranges::contains(values, value) ? right_child : left_child;
    }

    int get_state_id() const
    {
        assert(!is_split());
        return state_id;
    }

    friend std::ostream& operator<<(std::ostream& os, const Node& node);
};
} // namespace downward::cartesian_abstractions

#endif
