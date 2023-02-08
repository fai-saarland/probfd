#include "probfd/heuristics/pdbs/match_tree.h"

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

struct MatchTree::Node {
    static const int LEAF_NODE = -1;
    Node() = default;
    ~Node();
    vector<size_t> applicable_operator_ids;
    // The variable which this node represents.
    int var_id = LEAF_NODE;
    int var_multiplier = 0;
    int var_domain_size = 0;
    /*
      Each node has one outgoing edge for each possible value of the variable
      and one "star-edge" that is used when the value of the variable is
      undefined.
    */
    Node** successors = nullptr;
    Node* star_successor = nullptr;

    void initialize(int var_id, int var_multiplier, int var_domain_size);
    bool is_leaf_node() const;
};

MatchTree::Node::~Node()
{
    if (successors) {
        for (int i = 0; i < var_domain_size; ++i) {
            delete successors[i];
        }
        delete[] successors;
    }
    delete star_successor;
}

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
        successors = new Node*[var_domain_size];
        for (int val = 0; val < var_domain_size; ++val) {
            successors[val] = nullptr;
        }
    }
}

bool MatchTree::Node::is_leaf_node() const
{
    return var_id == LEAF_NODE;
}

MatchTree::MatchTree(ProbabilisticTaskProxy task_proxy, const Pattern& pattern)
    : task_proxy(task_proxy)
    , pattern(pattern)
    , root(nullptr)
{
}

MatchTree::~MatchTree()
{
    delete root;
}

void MatchTree::insert_recursive(
    const StateRankingFunction& mapper,
    const size_t op_index,
    const vector<FactPair>& progression_preconditions,
    int pre_index,
    Node** edge_from_parent)
{
    if (*edge_from_parent == 0) {
        // We don't exist yet: create a new node.
        *edge_from_parent = new Node();
    }

    Node* node = *edge_from_parent;
    if (pre_index == static_cast<int>(progression_preconditions.size())) {
        // All preconditions have been checked, insert operator ID.
        node->applicable_operator_ids.push_back(op_index);
    } else {
        const FactPair& fact = progression_preconditions[pre_index];
        int pattern_var_id = fact.var;
        int var_id = pattern[pattern_var_id];
        int var_multiplier = mapper.get_multiplier(pattern_var_id);

        VariablesProxy variables = task_proxy.get_variables();
        int var_domain_size = variables[var_id].get_domain_size();

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
            *edge_from_parent = new_node;
            new_node->star_successor = node;
            // The new node is now the node of interest.
            node = new_node;
        }

        /* Set up edge to the correct child (for which we want to call
           this function recursively). */
        Node** edge_to_child = 0;
        if (node->var_id == fact.var) {
            // Operator has a precondition on the variable tested by node.
            edge_to_child = &node->successors[fact.value];
            ++pre_index;
        } else {
            // Operator doesn't have a precondition on the variable tested by
            // node: follow/create the star-edge.
            assert(node->var_id < fact.var);
            edge_to_child = &node->star_successor;
        }

        insert_recursive(
            mapper,
            op_index,
            progression_preconditions,
            pre_index,
            edge_to_child);
    }
}

void MatchTree::insert(
    const StateRankingFunction& mapper,
    const size_t op_index,
    const vector<FactPair>& progression_preconditions)
{
    insert_recursive(mapper, op_index, progression_preconditions, 0, &root);
}

void MatchTree::get_applicable_operators_recursive(
    Node* node,
    const StateRank abstract_state,
    vector<const AbstractOperator*>& operator_ids) const
{
    /*
      Note: different from the code that builds the match tree, we do
      the test if node == 0 *before* calling traverse rather than *at
      the start* of traverse since this turned out to be faster in
      some informal experiments.
     */

    for (size_t op_index : node->applicable_operator_ids) {
        operator_ids.push_back(first + op_index);
    }

    if (node->is_leaf_node()) return;

    int temp = abstract_state.id / node->var_multiplier;
    int val = temp % node->var_domain_size;

    if (node->successors[val]) {
        // Follow the correct successor edge, if it exists.
        get_applicable_operators_recursive(
            node->successors[val],
            abstract_state,
            operator_ids);
    }
    if (node->star_successor) {
        // Always follow the star edge, if it exists.
        get_applicable_operators_recursive(
            node->star_successor,
            abstract_state,
            operator_ids);
    }
}

void MatchTree::get_applicable_operators(
    StateRank abstract_state,
    vector<const AbstractOperator*>& operator_ids) const
{
    if (root)
        get_applicable_operators_recursive(root, abstract_state, operator_ids);
}

void MatchTree::dump_recursive(Node* node) const
{
    if (!node) {
        // Node is the root node.
        std::cout << "Empty MatchTree" << endl;
        return;
    }
    std::cout << endl;
    std::cout << "node->var_id = " << node->var_id << endl;
    std::cout << "Number of applicable operators at this node: "
              << node->applicable_operator_ids.size() << endl;
    for (const size_t op_index : node->applicable_operator_ids) {
        std::cout << "AbstractOperator #" << op_index << endl;
    }
    if (node->is_leaf_node()) {
        std::cout << "leaf node." << endl;
        assert(!node->successors);
        assert(!node->star_successor);
    } else {
        for (int val = 0; val < node->var_domain_size; ++val) {
            if (node->successors[val]) {
                std::cout << "recursive call for child with value " << val
                          << endl;
                dump_recursive(node->successors[val]);
                std::cout << "back from recursive call (for successors[" << val
                          << "]) to node with var_id = " << node->var_id
                          << endl;
            } else {
                std::cout << "no child for value " << val << endl;
            }
        }
        if (node->star_successor) {
            std::cout << "recursive call for star_successor" << endl;
            dump_recursive(node->star_successor);
            std::cout << "back from recursive call (for star_successor) "
                      << "to node with var_id = " << node->var_id << endl;
        } else {
            std::cout << "no star_successor" << endl;
        }
    }
}

void MatchTree::set_first_aop(const AbstractOperator* first)
{
    this->first = first;
}

void MatchTree::dump() const
{
    dump_recursive(root);
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd