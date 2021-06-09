#include "state_reward_function.h"

#include "../plugin.h"

namespace probabilistic {

static PluginTypePlugin<GlobalStateRewardFunction>
    _plugin("GlobalStateRewardFunction", "");

} // namespace probabilistic
