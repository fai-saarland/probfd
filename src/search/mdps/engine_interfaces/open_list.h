#pragma once

#include "../types.h"
#include "../value_type.h"

#include <deque>

namespace probabilistic {

template<typename Action>
class OpenList {
public:
    using is_default_implementation = std::true_type;

    StateID pop()
    {
        const StateID res = queue_.back();
        queue_.pop_back();
        return res;
    }

    void push(const StateID& state_id) { queue_.push_back(state_id); }

    void push(
        const StateID&,
        const Action&,
        const value_type::value_t&,
        const StateID& state_id)
    {
        queue_.push_back(state_id);
    }

    unsigned size() const { return queue_.size(); }

    bool empty() const { return queue_.empty(); }

    void clear() { queue_.clear(); }

private:
    std::deque<StateID> queue_;
};

} // namespace probabilistic
