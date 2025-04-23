#ifndef GOAL_FACT_LIST_H
#define GOAL_FACT_LIST_H

#include "downward/fact_pair.h"
#include "downward/proxy_collection.h"

namespace downward {

class GoalsProxy;

class GoalFactList {
public:
    virtual ~GoalFactList() = default;

    virtual int get_num_goals() const = 0;
    virtual FactPair get_goal_fact(int index) const = 0;

    GoalsProxy get_goals() const;
};

class GoalsProxy : public ProxyCollection<GoalsProxy> {
    const GoalFactList* goal_fact_list;

public:
    explicit GoalsProxy(const GoalFactList& goal_fact_list)
        : goal_fact_list(&goal_fact_list)
    {
    }

    std::size_t size() const { return goal_fact_list->get_num_goals(); }

    FactPair operator[](std::size_t index) const
    {
        assert(index < size());
        return goal_fact_list->get_goal_fact(index);
    }
};

inline GoalsProxy GoalFactList::get_goals() const
{
    return GoalsProxy(*this);
}

} // namespace downward

#endif
