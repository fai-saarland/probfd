#include "probfd/heuristics/pdbs/match_tree.h"

#include "probfd/heuristics/pdbs/projection_operator.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "probfd/transition.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

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
    if (var_domain_size > 0) {
        successors.resize(var_domain_size);
    }
}

bool MatchTree::Node::is_leaf_node() const
{
    return var_id == LEAF_NODE;
}

MatchTree::MatchTree(size_t hint_num_operators)
{
    projection_operators.reserve(hint_num_operators);
}

MatchTree& MatchTree::operator=(MatchTree&&) noexcept = default;

MatchTree::~MatchTree() = default;

void MatchTree::insert_recursive(
    const VariablesProxy& task_variables,
    const StateRankingFunction& ranking_function,
    const size_t op_index,
    const vector<FactPair>& progression_preconditions,
    int pre_index,
    std::unique_ptr<Node>& node)
{
    if (!node) {
        // We don't exist yet: create a new node.
        node.reset(new Node());
    }

    if (pre_index == static_cast<int>(progression_preconditions.size())) {
        // All preconditions have been checked, insert operator ID.
        node->applicable_operator_ids.push_back(op_index);
    } else {
        const FactPair& fact = progression_preconditions[pre_index];
        int pattern_var_id = fact.var;
        int var_id = ranking_function.get_pattern()[pattern_var_id];
        int var_multiplier = ranking_function.get_multiplier(pattern_var_id);
        int var_domain_size = task_variables[var_id].get_domain_size();

        // Set up node correctly or insert a new node if necessary.
        if (node->is_leaf_node()) {
            node->initialize(pattern_var_id, var_multiplier, var_domain_size);
        } else if (node->var_id > pattern_var_id) {
            /* The variable to test has been left out: must insert new
               node and treat it as the "node". */
            Node* new_node = new Node();
            new_node->initialize(
                pattern_var_id,
                var_multiplier,
                var_domain_size);
            // The new node gets the left out variable as its variable.
            new_node->star_successor = std::move(node);
            node.reset(new_node);
            // The new node is now the node of interest.
        }

        /* Set up edge to the correct child (for which we want to call
           this function recursively). */
        if (node->var_id == fact.var) {
            // Operator has a precondition on the variable tested by node.
            ++pre_index;
            insert_recursive(
                task_variables,
                ranking_function,
                op_index,
                progression_preconditions,
                pre_index,
                node->successors[fact.value]);
        } else {
            // Operator doesn't have a precondition on the variable tested by
            // node: follow/create the star-edge.
            assert(node->var_id < fact.var);
            insert_recursive(
                task_variables,
                ranking_function,
                op_index,
                progression_preconditions,
                pre_index,
                node->star_successor);
        }
    }
}

void MatchTree::insert(
    const VariablesProxy& task_variables,
    const StateRankingFunction& ranking_function,
    ProjectionOperator op,
    const vector<FactPair>& progression_preconditions)
{
    insert_recursive(
        task_variables,
        ranking_function,
        projection_operators.size(),
        progression_preconditions,
        0,
        root);
    projection_operators.push_back(std::move(op));
}

void MatchTree::get_applicable_operators_recursive(
    Node* node,
    const StateRank abstract_state,
    vector<const ProjectionOperator*>& operator_ids) const
{
    /*
      Note: different from the code that builds the match tree, we do
      the test if node == 0 *before* calling traverse rather than *at
      the start* of traverse since this turned out to be faster in
      some informal experiments.
     */

    for (size_t op_index : node->applicable_operator_ids) {
        operator_ids.push_back(projection_operators.data() + op_index);
    }

    if (node->is_leaf_node()) return;

    int temp = abstract_state.id / node->var_multiplier;
    int val = temp % node->var_domain_size;

    if (node->successors[val]) {
        // Follow the correct successor edge, if it exists.
        get_applicable_operators_recursive(
            node->successors[val].get(),
            abstract_state,
            operator_ids);
    }
    if (node->star_successor) {
        // Always follow the star edge, if it exists.
        get_applicable_operators_recursive(
            node->star_successor.get(),
            abstract_state,
            operator_ids);
    }
}

void MatchTree::generate_all_transitions_recursive(
    Node* node,
    StateRank abstract_state,
    std::vector<Transition<const ProjectionOperator*>>& transitions,
    ProjectionStateSpace& state_space) const
{
    /*
      Note: different from the code that builds the match tree, we do
      the test if node == 0 *before* calling traverse rather than *at
      the start* of traverse since this turned out to be faster in
      some informal experiments.
     */

    for (size_t op_index : node->applicable_operator_ids) {
        auto* op = projection_operators.data() + op_index;
        auto& t = transitions.emplace_back(op);
        state_space.generate_action_transitions(
            abstract_state,
            op,
            t.successor_dist);
    }

    if (node->is_leaf_node()) return;

    int temp = abstract_state.id / node->var_multiplier;
    int val = temp % node->var_domain_size;

    if (node->successors[val]) {
        // Follow the correct successor edge, if it exists.
        generate_all_transitions_recursive(
            node->successors[val].get(),
            abstract_state,
            transitions,
            state_space);
    }
    if (node->star_successor) {
        // Always follow the star edge, if it exists.
        generate_all_transitions_recursive(
            node->star_successor.get(),
            abstract_state,
            transitions,
            state_space);
    }
}

void MatchTree::get_applicable_operators(
    StateRank abstract_state,
    vector<const ProjectionOperator*>& operator_ids) const
{
    if (root)
        get_applicable_operators_recursive(
            root.get(),
            abstract_state,
            operator_ids);
}

void MatchTree::generate_all_transitions(
    StateRank abstract_state,
    std::vector<Transition<const ProjectionOperator*>>& transitions,
    ProjectionStateSpace& state_space) const
{
    if (root)
        generate_all_transitions_recursive(
            root.get(),
            abstract_state,
            transitions,
            state_space);
}

const ProjectionOperator& MatchTree::get_index_operator(int index) const
{
    return projection_operators[index];
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
    dump_recursive(out, root.get());
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd