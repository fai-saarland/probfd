#include "downward/merge_and_shrink/merge_tree.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/system.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace downward::merge_and_shrink {
const int UNINITIALIZED = -1;

MergeTreeNode::MergeTreeNode(const MergeTreeNode& other)
    : parent(nullptr)
    , left_child(nullptr)
    , right_child(nullptr)
    , ts_index(other.ts_index)
{
    if (other.left_child) {
        left_child = new MergeTreeNode(*other.left_child);
    }
    if (other.right_child) {
        right_child = new MergeTreeNode(*other.right_child);
    }
}

MergeTreeNode::MergeTreeNode(int ts_index)
    : parent(nullptr)
    , left_child(nullptr)
    , right_child(nullptr)
    , ts_index(ts_index)
{
}

MergeTreeNode::MergeTreeNode(
    MergeTreeNode* left_child,
    MergeTreeNode* right_child)
    : parent(nullptr)
    , left_child(left_child)
    , right_child(right_child)
    , ts_index(UNINITIALIZED)
{
    left_child->parent = this;
    right_child->parent = this;
}

MergeTreeNode::~MergeTreeNode()
{
    delete left_child;
    delete right_child;
    left_child = nullptr;
    right_child = nullptr;
}

MergeTreeNode* MergeTreeNode::get_left_most_sibling()
{
    if (has_two_leaf_children()) {
        return this;
    }
    assert(left_child && right_child);

    if (!left_child->is_leaf()) {
        return left_child->get_left_most_sibling();
    }

    assert(!right_child->is_leaf());
    return right_child->get_left_most_sibling();
}

pair<int, int> MergeTreeNode::erase_children_and_set_index(int new_index)
{
    assert(has_two_leaf_children());
    int left_child_index = left_child->ts_index;
    int right_child_index = right_child->ts_index;
    assert(left_child_index != UNINITIALIZED);
    assert(right_child_index != UNINITIALIZED);
    delete left_child;
    delete right_child;
    left_child = nullptr;
    right_child = nullptr;
    assert(ts_index == UNINITIALIZED);
    ts_index = new_index;
    return make_pair(left_child_index, right_child_index);
}

MergeTreeNode* MergeTreeNode::get_parent_of_ts_index(int index)
{
    if (left_child && left_child->is_leaf() && left_child->ts_index == index) {
        return this;
    }

    if (right_child && right_child->is_leaf() &&
        right_child->ts_index == index) {
        return this;
    }

    MergeTreeNode* parent = nullptr;
    if (left_child) {
        parent = left_child->get_parent_of_ts_index(index);
    }
    if (parent) {
        return parent;
    }

    if (right_child) {
        parent = right_child->get_parent_of_ts_index(index);
    }
    return parent;
}

int MergeTreeNode::compute_num_internal_nodes() const
{
    if (is_leaf()) {
        return 0;
    } else {
        int number_of_internal_nodes = 1; // count the node itself
        if (left_child) {
            number_of_internal_nodes +=
                left_child->compute_num_internal_nodes();
        }
        if (right_child) {
            number_of_internal_nodes +=
                right_child->compute_num_internal_nodes();
        }
        return number_of_internal_nodes;
    }
}

void MergeTreeNode::inorder(
    int offset,
    int current_indentation,
    utils::LogProxy& log) const
{
    if (right_child) {
        right_child->inorder(offset, current_indentation + offset, log);
    }
    for (int i = 0; i < current_indentation; ++i) {
        log << " ";
    }
    log << ts_index << endl;
    if (left_child) {
        left_child->inorder(offset, current_indentation + offset, log);
    }
}

namespace {
class MergeUpdateStrategyUseFirst : public MergeUpdateStrategy {
public:
    std::pair<MergeTreeNode*, MergeTreeNode*> compute_surviving_removed_node(
        MergeTreeNode* first_parent,
        MergeTreeNode* second_parent) override
    {
        return {first_parent, second_parent};
    }

    void dump_options(utils::LogProxy& log) override
    {
        log << "use first";
    }
};

class MergeUpdateStrategyUseSecond : public MergeUpdateStrategy {
public:
    std::pair<MergeTreeNode*, MergeTreeNode*> compute_surviving_removed_node(
        MergeTreeNode* first_parent,
        MergeTreeNode* second_parent) override
    {
        return {second_parent, first_parent};
    }

    void dump_options(utils::LogProxy& log) override
    {
        log << "use second";
    }
};

class MergeUpdateStrategyUseRandom : public MergeUpdateStrategy {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit MergeUpdateStrategyUseRandom(
        std::shared_ptr<utils::RandomNumberGenerator> rng)
        : rng(std::move(rng))
    {
    }

    std::pair<MergeTreeNode*, MergeTreeNode*> compute_surviving_removed_node(
        MergeTreeNode* first_parent,
        MergeTreeNode* second_parent) override
    {
        if (rng->random(2)) {
            return {first_parent, second_parent};
        }

        return {second_parent, first_parent};
    }

    void dump_options(utils::LogProxy& log) override
    {
        log << "use random";
    }
};
} // namespace

std::unique_ptr<MergeUpdateStrategy> create_merge_update_strategy_use_first()
{
    return std::make_unique<MergeUpdateStrategyUseFirst>();
}

std::unique_ptr<MergeUpdateStrategy> create_merge_update_strategy_use_second()
{
    return std::make_unique<MergeUpdateStrategyUseSecond>();
}

std::unique_ptr<MergeUpdateStrategy> create_merge_update_strategy_use_random(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
{
    return std::make_unique<MergeUpdateStrategyUseRandom>(std::move(rng));
}

MergeTree::MergeTree(
    MergeTreeNode* root,
    std::shared_ptr<MergeUpdateStrategy> update_strategy)
    : root(root)
    , update_strategy(std::move(update_strategy))
{
}

MergeTree::~MergeTree()
{
    delete root;
    root = nullptr;
}

pair<int, int> MergeTree::get_next_merge(int new_index)
{
    MergeTreeNode* next_merge = root->get_left_most_sibling();
    return next_merge->erase_children_and_set_index(new_index);
}

pair<MergeTreeNode*, MergeTreeNode*> MergeTree::get_parents_of_ts_indices(
    const pair<int, int>& ts_indices,
    int new_index)
{
    int ts_index1 = ts_indices.first;
    int ts_index2 = ts_indices.second;
    bool use_first_index_for_first_parent = true;

    // Copy the tree and ask it for next merges until found both indices.
    MergeTreeNode* copy = new MergeTreeNode(*root);
    int found_indices = 0;
    while (!copy->is_leaf()) {
        MergeTreeNode* next_merge = copy->get_left_most_sibling();
        pair<int, int> merge =
            next_merge->erase_children_and_set_index(new_index);
        if (merge.first == ts_index1 || merge.second == ts_index1) {
            ++found_indices;
        }
        if (merge.first == ts_index2 || merge.second == ts_index2) {
            if (!found_indices) {
                use_first_index_for_first_parent = false;
            }
            ++found_indices;
        }
        if (found_indices == 2) {
            break;
        }
    }
    delete copy;

    pair<MergeTreeNode*, MergeTreeNode*> result = make_pair(nullptr, nullptr);
    if (use_first_index_for_first_parent) {
        result.first = root->get_parent_of_ts_index(ts_index1);
        result.second = root->get_parent_of_ts_index(ts_index2);
    } else {
        result.first = root->get_parent_of_ts_index(ts_index2);
        result.second = root->get_parent_of_ts_index(ts_index1);
    }
    return result;
}

void MergeTree::update(pair<int, int> merge, int new_index)
{
    int ts_index1 = merge.first;
    int ts_index2 = merge.second;
    assert(root->ts_index != ts_index1 && root->ts_index != ts_index2);

    auto [first_parent, second_parent] =
        get_parents_of_ts_indices(merge, new_index);

    if (first_parent == second_parent) { // given merge already in the tree
        first_parent->erase_children_and_set_index(new_index);
    } else {
        auto [surviving_node, removed_node] =
            update_strategy->compute_surviving_removed_node(
                first_parent,
                second_parent);

        // Update the leaf node corresponding to one of the indices to
        // correspond to the merged index.
        MergeTreeNode* surviving_leaf = nullptr;
        if (surviving_node->left_child->ts_index == ts_index1 ||
            surviving_node->left_child->ts_index == ts_index2) {
            surviving_leaf = surviving_node->left_child;
        } else {
            assert(
                surviving_node->right_child->ts_index == ts_index1 ||
                surviving_node->right_child->ts_index == ts_index2);
            surviving_leaf = surviving_node->right_child;
        }
        surviving_leaf->ts_index = new_index;

        // Remove all links to removed_node and store pointers to the
        // relevant children and its parent.
        MergeTreeNode* parent_of_removed_node = removed_node->parent;
        if (parent_of_removed_node) {
            // parent_of_removed_node can be nullptr if removed_node
            // is the root node
            if (parent_of_removed_node->left_child == removed_node) {
                parent_of_removed_node->left_child = nullptr;
            } else {
                assert(parent_of_removed_node->right_child == removed_node);
                parent_of_removed_node->right_child = nullptr;
            }
        }
        MergeTreeNode* surviving_child_of_removed_node = nullptr;
        /*
          Find the child of remove_node that should survive (i.e. the node that
          does not correspond to the merged indices) and set it to null so that
          deleting removed_node later does not delete (via destructor) the
          surviving child.
        */
        if (removed_node->left_child->ts_index == ts_index1 ||
            removed_node->left_child->ts_index == ts_index2) {
            surviving_child_of_removed_node = removed_node->right_child;

            removed_node->right_child = nullptr;
        } else {
            assert(
                removed_node->right_child->ts_index == ts_index1 ||
                removed_node->right_child->ts_index == ts_index2);
            surviving_child_of_removed_node = removed_node->left_child;
            removed_node->left_child = nullptr;
        }

        if (removed_node == root) {
            root = surviving_child_of_removed_node;
        }

        // Finally delete removed_node (this also deletes its child
        // corresponding to one of the merged indices, but not the other one).
        delete removed_node;
        removed_node = nullptr;

        // Update pointers of the surviving child of removed_node and the
        // parent of removed_node (if existing) to point to each other.
        surviving_child_of_removed_node->parent = parent_of_removed_node;
        if (parent_of_removed_node) {
            // parent_of_removed_node can be nullptr if removed_node
            // was the root node
            if (!parent_of_removed_node->left_child) {
                parent_of_removed_node->left_child =
                    surviving_child_of_removed_node;
            } else {
                assert(!parent_of_removed_node->right_child);
                parent_of_removed_node->right_child =
                    surviving_child_of_removed_node;
            }
        }
    }
}

void MergeTree::inorder_traversal(int indentation_offset, utils::LogProxy& log)
    const
{
    log << "Merge tree, read from left to right (90° rotated tree): " << endl;
    return root->inorder(indentation_offset, 0, log);
}
} // namespace downward::merge_and_shrink
