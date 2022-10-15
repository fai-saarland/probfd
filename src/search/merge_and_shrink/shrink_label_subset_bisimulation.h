#ifndef MERGE_AND_SHRINK_SHRINK_LABEL_SUBSET_BISIMULATION_H
#define MERGE_AND_SHRINK_SHRINK_LABEL_SUBSET_BISIMULATION_H

#include "merge_and_shrink/shrink_bisimulation.h"

namespace merge_and_shrink {

class ShrinkLabelSubsetBisimulation : public ShrinkBisimulation
{
    enum GoalLeading {
        ALL_ACTIONS,
        ALL_GOAL_LEADING,
        BACKWARD_PRUNED_GOAL_LEADING,
        GOAL_LEADING_FROM_THRESHOLD,
        GOAL_LEADING_FROM_THRESHOLD_BACKWARD_PRUNED,
        SHORTEST_PATH_RELEVANT_FROM_THRESHOLD,
        SHORTEST_PATH_RELEVANT_FROM_THRESHOLD_BACKWARD_PRUNED,
        PATH_RELEVANT_FROM_THRESHOLD,
        APPROXIMATED_BY_H_MAX,
        APPROXIMATED_BY_H_MAX_BACKWARD_PRUNED,
        APPROXIMATED_BY_H_FF
    };
    const GoalLeading goal_leading;
    bool goal_leading_threshold_reached;
    int max_states_before_catching;

    std::vector<bool> reg_labels;
    void dump_caught_labels_statistic() const;
public:
    ShrinkLabelSubsetBisimulation(const options::Options &opts);
    virtual ~ShrinkLabelSubsetBisimulation();

    virtual std::string name() const;
    virtual void dump_strategy_specific_options() const;

    virtual bool reduce_labels_before_shrinking() const;

//    virtual void shrink(Abstraction &abs, int target, bool force = false);
//    virtual void shrink_atomic(Abstraction &abs);
    virtual void shrink_before_merge(Abstraction &abs1, Abstraction &abs2);

    virtual bool is_backward_pruned() const
    {
        return goal_leading == BACKWARD_PRUNED_GOAL_LEADING
               || goal_leading == GOAL_LEADING_FROM_THRESHOLD_BACKWARD_PRUNED
               || goal_leading == APPROXIMATED_BY_H_MAX_BACKWARD_PRUNED;
    }

    virtual const std::vector<bool> &get_caught_labels_set() const;
    static void handle_option_defaults(options::Options &opts);

};

}

#endif
