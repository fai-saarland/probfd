#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/engine_interfaces/task_types.h"
#include "probfd/engine_interfaces/transition_sorter.h"

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

} // namespace probfd
