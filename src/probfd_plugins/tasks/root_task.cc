#include "downward_plugins/plugins/plugin.h"

#include "probfd/tasks/root_task.h"

#include "probfd/probabilistic_task.h"

using namespace utils;
using namespace probfd;

using namespace downward_plugins::plugins;

namespace {

class RootTaskFeature
    : public TypedFeature<ProbabilisticTask, ProbabilisticTask> {
public:
    RootTaskFeature()
        : TypedFeature("root_ppt")
    {
    }

    std::shared_ptr<ProbabilisticTask>
    create_component(const Options&, const Context&) const override
    {
        return probfd::tasks::g_root_task;
    }
};

FeaturePlugin<RootTaskFeature> _plugin;

} // namespace
