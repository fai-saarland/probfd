#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/engine_interfaces/transition_sorter.h"

#include "operator_id.h"
#include "task_proxy.h"

#include "plugin.h"

namespace probfd {

static PluginTypePlugin<TaskOpenList> _plugin_open_list("TaskOpenList", "");

static PluginTypePlugin<TaskNewStateObserver>
    _plugin_new_state_handler("TaskNewStateObserver", "");

static PluginTypePlugin<TaskCostFunction>
    _plugin_cost_function("TaskCostFunction", "");

static PluginTypePlugin<TaskEvaluator>
    _plugin_state_evaluator("TaskEvaluator", "");

} // namespace probfd
