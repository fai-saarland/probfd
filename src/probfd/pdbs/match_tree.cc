#include "probfd/pdbs/match_tree.h"

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/state_ranking_function.h"

#include "probfd/transition.h"

#include "downward/abstract_task.h"

#include <cassert>
#include <ostream>
#include <utility>

using namespace std;

namespace probfd::pdbs {

struct MatchTree::Node {
    static const int LEAF_NODE = -1;
    std::vector<size_t> applicable_operator_ids;
    // The variable which this node represents.
    int var_id = LEAF_NODE;
    int var_multiplier = 0;
    int var_domain_size = 0;
    /*
      Each node has one outgoing edge for each possible value of the variable
      and one "star-edge" that is used when the value of the variable is
      undefined.
    */
    std::vector<std::unique_ptr<Node>> successors;
    std::unique_ptr<Node> star_successor;

    void initialize(int var_id, int var_multiplier, int var_domain_size);
    [[nodiscard]]
    bool is_leaf_node() const;
};

void MatchTree::Node::initialize(
    int var_id_,
    int var_multiplier_,
    int var_domain_size_)
{
    assert(is_leaf_node());
    assert(var_id_ >= 0);
    var_id = var_id_;
    var_multiplier = var_multiplier_;
    var_domain_size = var_domain_size_;
    successors.resize(var_domain_size);
}

bool MatchTree::Node::is_leaf_node() const
{
    return var_id == LEAF_NODE;
}

MatchTree::MatchTree(size_t hint_num_operators)
{
    projection_operators_.reserve(hint_num_operators);
}

MatchTree::~MatchTree() = default;

void MatchTree::insert(
    const AssignmentEnumerator& enumerator,
    ProjectionOperator op,
    const vector<FactPair>& progression_preconditions,
    FDRSimpleCostFunction* task_cost_function)
{
    std::unique_ptr<Node>* node = &root_;
    auto precondition_it = progression_preconditions.begin();
    const auto precondition_end = progression_preconditions.end();

    for (;;) {
        if (!node->get()) {
            // We don't exist yet: create a new node.
            *node = std::make_unique<Node>();
        }

        if (precondition_it == precondition_end) break;

        const FactPair& fact = *precondition_it;
        int pattern_var_id = fact.var;
        int var_multiplier = enumerator.get_multiplier(pattern_var_id);
        int var_domain_size = enumerator.get_domain_size(pattern_var_id);

        // Set up node correctly or insert a new node if necessary.
        if ((*node)->is_leaf_node()) {
            (*node)->initialize(
                pattern_var_id,
                var_multiplier,
                var_domain_size);
        } else if ((*node)->var_id > pattern_var_id) {
            /* The variable to test has been left out: must insert new
               node and treat it as the "node". */
            auto new_node = std::make_unique<Node>();
            new_node->initialize(
                pattern_var_id,
                var_multiplier,
                var_domain_size);
            // The new node gets the left out variable as its variable.
            new_node->star_successor = std::move(*node);
            *node = std::move(new_node);
            // The new node is now the node of interest.
        } else if ((*node)->var_id < pattern_var_id) {
            // Operator doesn't have a precondition on the variable tested
            // by node: follow the star-edge.
            assert((*node)->var_id < fact.var);
            node = &(*node)->star_successor;
            continue;
        }

        // Operator has a precondition on the variable tested by node.
        ++precondition_it;
        node = &(*node)->successors[fact.value];
    }

    // If a cost function is passed, merge equivalent operators and take
    // the minimum cost.
    if (task_cost_function) {
        const auto cost = task_cost_function->get_action_cost(op.operator_id);
        for (std::size_t op_id : (*node)->applicable_operator_ids) {
            ProjectionOperator& other = projection_operators_[op_id];
            if (!are_equivalent(op, other)) continue;
            if (cost >= task_cost_function->get_action_cost(other.operator_id))
                return;
            other.operator_id = op.operator_id;
        }
    }

    (*node)->applicable_operator_ids.push_back(projection_operators_.size());
    projection_operators_.push_back(std::move(op));
}

void MatchTree::get_applicable_operators(
    StateRank abstract_state_rank,
    vector<const ProjectionOperator*>& operator_ids) const
{
    if (!root_) return;

    assert(nodes_.empty());
    nodes_.push(root_.get());

    while (!nodes_.empty()) {
        Node* node = nodes_.top();
        nodes_.pop();

        for (size_t op_index : node->applicable_operator_ids) {
            operator_ids.push_back(projection_operators_.data() + op_index);
        }

        if (node->is_leaf_node()) continue;

        int temp = abstract_state_rank / node->var_multiplier;
        int val = temp % node->var_domain_size;

        if (node->star_successor) {
            // Always follow the star edge, if it exists.
            nodes_.push(node->star_successor.get());
        }

        if (node->successors[val]) {
            // Follow the correct successor edge, if it exists.
            nodes_.push(node->successors[val].get());
        }
    }
}

void MatchTree::generate_all_transitions(
    StateRank abstract_state_rank,
    std::vector<Transition<const ProjectionOperator*>>& transitions,
    ProjectionStateSpace& state_space) const
{
    if (!root_) return;

    /*
      Note: different from the code that builds the match tree, we do
      the test if node == 0 *before* calling traverse rather than *at
      the start* of traverse since this turned out to be faster in
      some informal experiments.
     */

    assert(nodes_.empty());
    nodes_.push(root_.get());

    while (!nodes_.empty()) {
        Node* node = nodes_.top();
        nodes_.pop();

        for (size_t op_index : node->applicable_operator_ids) {
            auto* op = projection_operators_.data() + op_index;
            auto& t = transitions.emplace_back(op);
            state_space.generate_action_transitions(
                abstract_state_rank,
                op,
                t.successor_dist);
        }

        if (node->is_leaf_node()) continue;

        int temp = abstract_state_rank / node->var_multiplier;
        int val = temp % node->var_domain_size;

        if (node->star_successor) {
            // Always follow the star edge, if it exists.
            nodes_.push(node->star_successor.get());
        }

        if (node->successors[val]) {
            // Follow the correct successor edge, if it exists.
            nodes_.push(node->successors[val].get());
        }
    }
}

void MatchTree::dump_recursive(std::ostream& out, Node* node) const
{
    if (!node) {
        // Node is the root node.
        out << "Empty MatchTree" << endl;
        return;
    }
    out << endl;
    out << "node->var_id = " << node->var_id << endl;
    out << "Number of applicable operators at this node: "
        << node->applicable_operator_ids.size() << endl;
    for (const size_t op_index : node->applicable_operator_ids) {
        out << "ProjectionOperator #" << op_index << endl;
    }
    if (node->is_leaf_node()) {
        out << "leaf node." << endl;
        assert(node->successors.empty());
        assert(!node->star_successor);
    } else {
        for (int val = 0; val < node->var_domain_size; ++val) {
            if (node->successors[val]) {
                out << "recursive call for child with value " << val << endl;
                dump_recursive(out, node->successors[val].get());
                out << "back from recursive call (for successors[" << val
                    << "]) to node with var_id = " << node->var_id << endl;
            } else {
                out << "no child for value " << val << endl;
            }
        }
        if (node->star_successor) {
            out << "recursive call for star_successor" << endl;
            dump_recursive(out, node->star_successor.get());
            out << "back from recursive call (for star_successor) "
                << "to node with var_id = " << node->var_id << endl;
        } else {
            out << "no star_successor" << endl;
        }
    }
}

void MatchTree::dump(std::ostream& out) const
{
    dump_recursive(out, root_.get());
}

} // namespace probfd::pdbs
