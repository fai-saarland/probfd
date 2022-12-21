#include "legacy/merge_and_shrink/label_reducer.h"

#include "legacy/global_operator.h"
#include "legacy/globals.h"

#include "utils/hash.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

using namespace std;

namespace legacy {
namespace merge_and_shrink {

typedef pair<int, int> Assignment;

struct OperatorSignature {
    vector<int> data;

    OperatorSignature(
        const vector<Assignment>& preconditions,
        const vector<Assignment>& effects,
        int cost,
        bool label_caught)
    {
        // We require that preconditions and effects are sorted by
        // variable -- some sort of canonical representation is needed
        // to guarantee that we can properly test for uniqueness.
        for (size_t i = 0; i < preconditions.size(); ++i) {
            if (i != 0) {
                assert(preconditions[i].first > preconditions[i - 1].first);
            }
            data.push_back(preconditions[i].first);
            data.push_back(preconditions[i].second);
        }
        data.push_back(-1); // marker
        for (size_t i = 0; i < effects.size(); ++i) {
            if (i != 0) {
                assert(effects[i].first > effects[i - 1].first);
            }
            data.push_back(effects[i].first);
            data.push_back(effects[i].second);
        }
        data.push_back(-1); // marker
        data.push_back(cost);
        // For distinguishing between caught and not caught labels
        data.push_back(label_caught);
    }

    bool operator==(const OperatorSignature& other) const
    {
        return data == other.data;
    }

    size_t hash() const { return utils::get_hash(data); }
};

} // namespace merge_and_shrink
} // namespace legacy

namespace std {
template <>
struct hash<legacy::merge_and_shrink::OperatorSignature> {
    size_t
    operator()(const legacy::merge_and_shrink::OperatorSignature& sig) const
    {
        return sig.hash();
    }
};
} // namespace std

namespace legacy {
namespace merge_and_shrink {

LabelReducer::LabelReducer(
    const vector<const GlobalOperator*>& relevant_operators,
    const vector<int>& pruned_vars,
    OperatorCost cost_type,
    const std::vector<bool>& caught_labels,
    vector<set<int>>& vars_label_operators)
{
    empty_label_operators.resize(g_operators.size(), false);
    vars_label_operators.resize(g_operators.size());
    num_pruned_vars = pruned_vars.size();
    num_labels = relevant_operators.size();
    num_reduced_labels = 0;

    vector<bool> var_is_used(g_variable_domain.size(), true);
    for (size_t i = 0; i < pruned_vars.size(); ++i) {
        var_is_used[pruned_vars[i]] = false;
    }

    std::unordered_map<OperatorSignature, const GlobalOperator*>
        reduced_label_map;
    reduced_label_by_index.resize(g_operators.size(), 0);

    for (size_t i = 0; i < relevant_operators.size(); ++i) {
        const GlobalOperator* op = relevant_operators[i];
        int op_index = get_op_index(op);

        bool caught = caught_labels.size() == 0 || caught_labels[op_index];
        bool is_empty_label_operator;
        vector<int> label_vars;
        OperatorSignature signature = build_operator_signature(
            *op,
            cost_type,
            var_is_used,
            caught,
            is_empty_label_operator,
            vars_label_operators[op_index]);
        empty_label_operators[op_index] = is_empty_label_operator;

        if (!reduced_label_map.count(signature)) {
            reduced_label_map[signature] = op;
            reduced_label_by_index[op_index] = op;
            ++num_reduced_labels;
        } else {
            reduced_label_by_index[op_index] = reduced_label_map[signature];
        }
    }
    assert(reduced_label_map.size() == (size_t)num_reduced_labels);
}

LabelReducer::~LabelReducer()
{
}

OperatorSignature LabelReducer::build_operator_signature(
    const GlobalOperator& op,
    OperatorCost cost_type,
    const vector<bool>& var_is_used,
    bool label_caught,
    bool& is_empty_label_operator,
    set<int>& vars_label_operator) const
{
    vector<Assignment> preconditions;
    vector<Assignment> effects;

    is_empty_label_operator = true;
    int op_cost = get_adjusted_action_cost(op, cost_type);
    const auto& precond = op.get_preconditions();
    for (size_t i = 0; i < precond.size(); ++i) {
        int var = precond[i].var;
        if (var_is_used[var]) {
            int val = precond[i].val;
            preconditions.push_back(make_pair(var, val));
            is_empty_label_operator = false;
            vars_label_operator.insert(var);
        }
    }
    const auto& pre_post = op.get_effects();
    for (size_t i = 0; i < pre_post.size(); ++i) {
        int var = pre_post[i].var;
        if (var_is_used[var]) {
            int post = pre_post[i].val;
            effects.push_back(make_pair(var, post));
            is_empty_label_operator = false;
            vars_label_operator.insert(var);
        }
    }
    std::sort(preconditions.begin(), preconditions.end());
    std::sort(effects.begin(), effects.end());

    return OperatorSignature(preconditions, effects, op_cost, label_caught);
}

void LabelReducer::statistics() const
{
    cout << "Label reduction: " << num_pruned_vars << " pruned vars, "
         << num_labels << " labels, " << num_reduced_labels << " reduced labels"
         << endl;
}

} // namespace merge_and_shrink
} // namespace legacy