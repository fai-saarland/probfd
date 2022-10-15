#ifndef MDPS_NEW_STATE_HANDLER_H
#define MDPS_NEW_STATE_HANDLER_H

#include "probfd/engine_interfaces/new_state_handler.h"

#include "global_state.h"

#include <memory>
#include <vector>

namespace probfd {

using NewGlobalStateHandler = engine_interfaces::NewStateHandler<GlobalState>;
using NewGlobalStateHandlerList =
    engine_interfaces::NewStateHandlerList<GlobalState>;

} // namespace probfd

#endif // __NEW_STATE_HANDLER_H__