#pragma once

#include "../global_state.h"
#include "algorithms/types_heuristic_search.h"
#include "probabilistic_operator.h"

#include <memory>
#include <vector>

namespace probabilistic {

using NewGlobalStateHandler =
    algorithms::NewStateHandler<GlobalState, const ProbabilisticOperator*>;

class NewGlobalStateHandlerList : public NewGlobalStateHandler {
public:
    NewGlobalStateHandlerList(
        const std::vector<std::shared_ptr<NewGlobalStateHandler>>& handlers);
    virtual void touch(const GlobalState& s) override;
    virtual void touch_dead_end(const GlobalState& s) override;
    virtual void touch_goal(const GlobalState& s) override;

private:
    std::vector<std::shared_ptr<NewGlobalStateHandler>> handlers_;
};

} // namespace probabilistic

