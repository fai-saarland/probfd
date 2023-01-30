#include "probfd/engine_interfaces/new_state_handler.h"
#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/reward_function.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/successor_sorter.h"
#include "probfd/engine_interfaces/transition_sampler.h"

#include "operator_id.h"
#include "task_proxy.h"

#include "plugin.h"

namespace probfd {

static PluginTypePlugin<TaskOpenList> _plugin_open_list("TaskOpenList", "");

static PluginTypePlugin<TaskNewStateHandler>
    _plugin_new_state_handler("TaskNewStateHandler", "");

static PluginTypePlugin<TaskRewardFunction>
    _plugin_reward_function("TaskRewardFunction", "");

static PluginTypePlugin<TaskStateEvaluator>
    _plugin_state_evaluator("TaskStateEvaluator", "");

} // namespace probfd
