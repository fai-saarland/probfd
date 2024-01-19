#ifndef PROBFD_OPEN_LISTS_LIFO_OPEN_LIST_H
#define PROBFD_OPEN_LISTS_LIFO_OPEN_LIST_H

#include "probfd/algorithms/open_list.h"

#include <deque>

namespace probfd::open_lists {

template <typename Action>
class LifoOpenList : public algorithms::OpenList<Action> {
    std::deque<StateID> queue_;

public:
    [[nodiscard]]
    bool empty() const override
    {
        return queue_.empty();
    }

    [[nodiscard]]
    unsigned size() const override
    {
        return queue_.size();
    }

    StateID pop() override
    {
        StateID s = queue_.back();
        queue_.pop_back();
        return s;
    }

    void push(StateID state_id) override { queue_.push_back(state_id); }

    void clear() override { queue_.clear(); }
};

} // namespace probfd::open_lists

#endif // PROBFD_OPEN_LISTS_LIFO_OPEN_LIST_H