#ifndef MERGE_AND_SHRINK_SHRINK_NONE_H
#define MERGE_AND_SHRINK_SHRINK_NONE_H

#include "merge_and_shrink/shrink_strategy.h"

namespace merge_and_shrink {

class ShrinkNone : public ShrinkStrategy
{
public:
    // Michael: Moved from bisimulation
    enum GoalLeading {
        NO_GOAL_LEADING,
        ALL_GOAL_LEADING,
        BACKWARD_PRUNED_GOAL_LEADING
    };

private:
    const GoalLeading goal_leading;

public:
    ShrinkNone(const options::Options &opts);
    virtual ~ShrinkNone();

    virtual std::string name() const;
    virtual void dump_strategy_specific_options() const;

    virtual bool reduce_labels_before_shrinking() const;

    virtual void shrink(Abstraction &abs, int target, bool force = false);
    virtual void shrink_atomic(Abstraction &abs);
    virtual void shrink_before_merge(Abstraction &abs1, Abstraction &abs2);

    virtual bool is_goal_leading() const
    {
        return goal_leading != NO_GOAL_LEADING;
    }
    virtual bool is_backward_pruned() const
    {
        return goal_leading == BACKWARD_PRUNED_GOAL_LEADING;
    }

};

}

#endif
