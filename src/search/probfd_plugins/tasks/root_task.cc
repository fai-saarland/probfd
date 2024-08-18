#include "probfd/tasks/root_task.h"

#include "probfd/probabilistic_task.h"

#include "downward/plugins/plugin.h"

using namespace plugins;

using namespace probfd;

namespace {

class RootTaskFeature
    : public TypedFeature<ProbabilisticTask, ProbabilisticTask> {
public:
    RootTaskFeature()
        : TypedFeature("root_ppt")
    {
    }

    std::shared_ptr<ProbabilisticTask>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return probfd::tasks::g_root_task;
    }
};

plugins::FeaturePlugin<RootTaskFeature> _plugin;

} // namespace
