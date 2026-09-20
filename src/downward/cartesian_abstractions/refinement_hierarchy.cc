#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/state.h"

using namespace std;

namespace downward::cartesian_abstractions {
Node::Node(int state_id)
    : left_child(UNDEFINED)
    , right_child(UNDEFINED)
    , var(UNDEFINED)
    , state_id(state_id)
{
    assert(state_id != UNDEFINED);
    assert(!is_split());
}

bool Node::information_is_valid() const
{
    return (left_child == UNDEFINED && right_child == UNDEFINED &&
            var == UNDEFINED && values.empty() && state_id != UNDEFINED) ||
           (left_child != UNDEFINED && right_child != UNDEFINED &&
            var != UNDEFINED && !values.empty() && state_id == UNDEFINED);
}

bool Node::is_split() const
{
    assert(information_is_valid());
    return left_child != UNDEFINED;
}

void Node::split(
    int var,
    std::vector<int> values,
    NodeID left_child,
    NodeID right_child)
{
    this->var = var;
    this->values = std::move(values);
    this->left_child = left_child;
    this->right_child = right_child;
    state_id = UNDEFINED;
    assert(is_split());
}

ostream& operator<<(ostream& os, const Node& node)
{
    std::print(
        os,
        "<Node: var={} values={} state={} left={} right={}>",
        node.var,
        node.values,
        node.state_id,
        node.left_child,
        node.right_child);

    return os;
}

RefinementHierarchy::RefinementHierarchy()
{
    nodes.emplace_back(0);
}

NodeID RefinementHierarchy::add_node(int state_id)
{
    NodeID node_id = nodes.size();
    nodes.emplace_back(state_id);
    return node_id;
}

pair<NodeID, NodeID> RefinementHierarchy::split(
    NodeID node_id,
    int var,
    const vector<int>& values,
    int left_state_id,
    int right_state_id)
{
    const NodeID left_child_id = add_node(left_state_id);
    const NodeID right_child_id = add_node(right_state_id);

    nodes[node_id].split(var, values, left_child_id, right_child_id);

    return make_pair(left_child_id, right_child_id);
}

int RefinementHierarchy::get_abstract_state_id(const State& state) const
{
    NodeID id = 0;
    while (nodes[id].is_split()) {
        const Node& node = nodes[id];
        id = node.get_child(state[node.get_var()]);
    }
    return nodes[id].get_state_id();
}
} // namespace downward::cartesian_abstractions
