#include "new_state_handler.h"

#include "../plugin.h"

namespace probabilistic {

NewGlobalStateHandlerList::NewGlobalStateHandlerList(
    const std::vector<std::shared_ptr<NewGlobalStateHandler>>& handlers)
    : handlers_(handlers)
{
}

void
NewGlobalStateHandlerList::touch(const GlobalState& s)
{
    for (unsigned i = 0; i < handlers_.size(); ++i) {
        handlers_[i]->touch(s);
    }
}

void
NewGlobalStateHandlerList::touch_dead_end(const GlobalState& s)
{
    for (unsigned i = 0; i < handlers_.size(); ++i) {
        handlers_[i]->touch_dead_end(s);
    }
}

void
NewGlobalStateHandlerList::touch_goal(const GlobalState& s)
{
    for (unsigned i = 0; i < handlers_.size(); ++i) {
        handlers_[i]->touch_goal(s);
    }
}

static PluginTypePlugin<NewGlobalStateHandler>
    _plugin("NewStateHandler", "", "on-new-state");

} // namespace probabilistic
