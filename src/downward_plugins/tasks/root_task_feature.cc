#include "downward_plugins/plugins/plugin.h"

#include "downward/tasks/root_task.h"

using namespace std;
using namespace tasks;
using namespace downward_plugins::plugins;

namespace {

class RootTaskFeature : public TypedFeature<AbstractTask, AbstractTask> {
public:
    RootTaskFeature()
        : TypedFeature("no_transform")
    {
    }

    [[nodiscard]]
    shared_ptr<AbstractTask>
    create_component(const Options&, const utils::Context&) const override
    {
        return g_root_task;
    }
};

FeaturePlugin<RootTaskFeature> plugin;

} // namespace
