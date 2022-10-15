#include "probfd/new_state_handler.h"

#include "plugin.h"

namespace probfd {

static PluginTypePlugin<NewGlobalStateHandler>
    _plugin("NewStateHandler", "", "on-new-state");

} // namespace probfd
