#include "new_state_handler.h"

#include "../plugin.h"

namespace probabilistic {

static PluginTypePlugin<NewGlobalStateHandler>
    _plugin("NewStateHandler", "", "on-new-state");

} // namespace probabilistic
