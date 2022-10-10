#include "reward_function.h"

#include "../plugin.h"

namespace probabilistic {

static PluginTypePlugin<GlobalRewardFunction>
    _plugin("GlobalRewardFunction", "");

} // namespace probabilistic
