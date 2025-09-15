#ifndef GOAL_FACT_LIST_H
#define GOAL_FACT_LIST_H

#include "downward/algorithms/subscriber.h"

#include "downward/fact_pair.h"
#include "downward/proxy_collection.h"

namespace downward {

class GoalFactList
    : public ProxyCollectionTag
    , public subscriber::SubscriberService<GoalFactList>
    , public std::ranges::view_interface<GoalFactList> {
public:
    virtual ~GoalFactList() = default;

    virtual int get_num_goals() const = 0;
    virtual FactPair get_goal_fact(int index) const = 0;

    std::size_t size() const { return this->get_num_goals(); }

    FactPair operator[](std::size_t index) const
    {
        assert(index < size());
        return this->get_goal_fact(index);
    }
};

} // namespace downward

#endif
