#include "probfd/reward_model.h"

#include "plugin.h"

namespace probfd {

std::shared_ptr<RewardModel> g_reward_model;

static PluginTypePlugin<RewardModel> _plugin_type("RewardModel", "");

} // namespace probfd
