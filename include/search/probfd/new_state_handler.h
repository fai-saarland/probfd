#ifndef MDPS_NEW_STATE_HANDLER_H
#define MDPS_NEW_STATE_HANDLER_H

#include "probfd/engine_interfaces/new_state_handler.h"

#include "legacy/global_state.h"

#include <memory>
#include <vector>

namespace probfd {

using NewGlobalStateHandler =
    engine_interfaces::NewStateHandler<legacy::GlobalState>;
using NewGlobalStateHandlerList =
    engine_interfaces::NewStateHandlerList<legacy::GlobalState>;

} // namespace probfd

#endif // __NEW_STATE_HANDLER_H__