#include "reward_function.h"

#include "../plugin.h"

namespace probfd {

static PluginTypePlugin<GlobalRewardFunction>
    _plugin("GlobalRewardFunction", "");

} // namespace probfd
