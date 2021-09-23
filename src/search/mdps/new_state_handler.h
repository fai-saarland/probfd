#ifndef MDPS_NEW_STATE_HANDLER_H
#define MDPS_NEW_STATE_HANDLER_H

#include "../global_state.h"
#include "engine_interfaces/new_state_handler.h"

#include <memory>
#include <vector>

namespace probabilistic {

template <>
class NewStateHandler<GlobalState> {
public:
    virtual ~NewStateHandler() = default;
    virtual void touch(const GlobalState&) {}
    virtual void touch_dead_end(const GlobalState&) {}
    virtual void touch_goal(const GlobalState&) {}
};

using NewGlobalStateHandler = NewStateHandler<GlobalState>;

class NewGlobalStateHandlerList : public NewGlobalStateHandler {
public:
    NewGlobalStateHandlerList(
        std::vector<std::shared_ptr<NewGlobalStateHandler>> handlers);
    virtual void touch(const GlobalState& s) override;
    virtual void touch_dead_end(const GlobalState& s) override;
    virtual void touch_goal(const GlobalState& s) override;

private:
    std::vector<std::shared_ptr<NewGlobalStateHandler>> handlers_;
};

} // namespace probabilistic

#endif // __NEW_STATE_HANDLER_H__