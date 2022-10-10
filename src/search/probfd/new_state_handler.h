#ifndef MDPS_NEW_STATE_HANDLER_H
#define MDPS_NEW_STATE_HANDLER_H

#include "../global_state.h"
#include "engine_interfaces/new_state_handler.h"

#include <memory>
#include <vector>

namespace probfd {

using NewGlobalStateHandler = NewStateHandler<GlobalState>;
using NewGlobalStateHandlerList = NewStateHandlerList<GlobalState>;

} // namespace probfd

#endif // __NEW_STATE_HANDLER_H__