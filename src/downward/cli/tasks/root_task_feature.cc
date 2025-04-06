#include "downward/cli/plugins/plugin.h"

#include "downward/tasks/root_task.h"

using namespace std;
using namespace downward::tasks;
using namespace downward::cli::plugins;

namespace {

class RootTaskFeature
    : public TypedFeature<downward::AbstractTask, downward::AbstractTask> {
public:
    RootTaskFeature()
        : TypedFeature("no_transform")
    {
    }

    [[nodiscard]]
    shared_ptr<downward::AbstractTask>
    create_component(const Options&, const downward::utils::Context&)
        const override
    {
        return g_root_task;
    }
};

FeaturePlugin<RootTaskFeature> plugin;

} // namespace
