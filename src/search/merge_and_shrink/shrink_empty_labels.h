#ifndef MERGE_AND_SHRINK_SHRINK_EMPTY_LABELS_H
#define MERGE_AND_SHRINK_SHRINK_EMPTY_LABELS_H

#include "shrink_strategy.h"
#include <vector>
#include <memory>

namespace merge_and_shrink {

class ShrinkEmptyLabels : public ShrinkStrategy
{
    void find_sccs(
        std::vector<int> &stack,
        std::vector<bool> &in_stack,
        EquivalenceRelation &final_sccs,
        const std::vector<std::vector<int> > &adjacency_matrix,
        std::vector<int> &indices,
        std::vector<int> &lowlinks,
        int &index,
        const int state,
        const bool all_goal_vars_in,
        std::vector<bool> &is_goal,
        std::vector<int> &state_to_scc);

public:
    ShrinkEmptyLabels(const options::Options &opts);
    virtual ~ShrinkEmptyLabels();

    virtual std::string name() const;
    virtual void dump_strategy_specific_options() const;

    virtual bool reduce_labels_before_shrinking() const;

    virtual void shrink(Abstraction &abs, int target, bool force = false);
    virtual void shrink_atomic(Abstraction &abs);
    virtual void shrink_before_merge(Abstraction &abs1, Abstraction &abs2);

    static std::shared_ptr<ShrinkEmptyLabels> create_default();

private:
    void determine_empty_label_operators(Abstraction &abs, std::vector<bool> &operators);

};

}

#endif
