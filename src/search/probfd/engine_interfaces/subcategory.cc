#include "probfd/engine_interfaces/fdr_types.h"
#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/engine_interfaces/transition_sorter.h"

#include "downward/plugins/plugin.h"

namespace probfd {

static class FDROpenListCategoryPlugin
    : public plugins::TypedCategoryPlugin<FDROpenList> {
public:
    FDROpenListCategoryPlugin()
        : TypedCategoryPlugin("FDROpenList")
    {
    }
} _category_plugin_open_list;

static class FDRNewStateObserverCategoryPlugin
    : public plugins::TypedCategoryPlugin<FDRNewStateObserver> {
public:
    FDRNewStateObserverCategoryPlugin()
        : TypedCategoryPlugin("FDRNewStateObserver")
    {
    }
} _category_plugin_new_state_observer;

} // namespace probfd
