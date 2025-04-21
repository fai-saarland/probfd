#ifndef GOAL_FACT_LIST_H
#define GOAL_FACT_LIST_H

#include "downward/fact_pair.h"

namespace downward {

class GoalFactList {
public:
    virtual ~GoalFactList() = default;

    virtual int get_num_goals() const = 0;
    virtual FactPair get_goal_fact(int index) const = 0;
};

} // namespace downward

#endif
