#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/engine_interfaces/transition_sorter.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include "downward/plugins/plugin.h"

namespace probfd {

static class TaskOpenListCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskOpenList> {
public:
    TaskOpenListCategoryPlugin()
        : TypedCategoryPlugin("TaskOpenList")
    {
    }
} _category_plugin_open_list;

static class TaskNewStateObserverCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskNewStateObserver> {
public:
    TaskNewStateObserverCategoryPlugin()
        : TypedCategoryPlugin("TaskNewStateObserver")
    {
    }
} _category_plugin_new_state_observer;

static class TaskCostFunctionCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskCostFunction> {
public:
    TaskCostFunctionCategoryPlugin()
        : TypedCategoryPlugin("TaskCostFunction")
    {
    }
} _category_plugin_cost_function;

static class TaskEvaluatorCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskEvaluator> {
public:
    TaskEvaluatorCategoryPlugin()
        : TypedCategoryPlugin("TaskEvaluator")
    {
    }
} _category_plugin_evaluator;

} // namespace probfd
