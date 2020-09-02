#ifndef MERGE_AND_SHRINK_LABEL_REDUCER_H
#define MERGE_AND_SHRINK_LABEL_REDUCER_H

#include "../global_operator.h"
#include "../globals.h"
#include "../operator_cost.h"

#include <cassert>
#include <set>
#include <vector>

namespace merge_and_shrink {

struct OperatorSignature;

class LabelReducer {
    std::vector<const GlobalOperator*> reduced_label_by_index;

    // TODO: Now that we are collecting the variables that are relevant
    // for a given operator, empty_label_operators[o] <=>
    //! vars_label_operator.empty()
    std::vector<bool> empty_label_operators;

    int num_pruned_vars;
    int num_labels;
    int num_reduced_labels;

    OperatorSignature build_operator_signature(
        const GlobalOperator& op,
        OperatorCost cost_type,
        const std::vector<bool>& var_is_used,
        bool label_caught,
        bool& is_empty_label_operator,
        std::set<int>& vars_label_operator) const;

public:
    LabelReducer(
        const std::vector<const GlobalOperator*>& relevant_operators,
        const std::vector<int>& pruned_vars,
        OperatorCost cost_type,
        const std::vector<bool>& caught_labels,
        std::vector<std::set<int> >& vars_label_operators);
    ~LabelReducer();
    inline int get_op_index(const GlobalOperator* op) const;
    inline const GlobalOperator*
    get_reduced_label(const GlobalOperator* op) const;
    void statistics() const;
    inline bool get_empty_label_operators(const int index) const;
};

inline int
LabelReducer::get_op_index(const GlobalOperator* op) const
{
    int op_index = op - &*g_operators.begin();
    assert(op_index >= 0 && (size_t)op_index < g_operators.size());
    return op_index;
}

inline const GlobalOperator*
LabelReducer::get_reduced_label(const GlobalOperator* op) const
{
    const GlobalOperator* reduced_label =
        reduced_label_by_index[get_op_index(op)];
    assert(reduced_label);
    return reduced_label;
}

inline bool
LabelReducer::get_empty_label_operators(const int index) const
{
    return empty_label_operators[index];
}

} // namespace merge_and_shrink

#endif
