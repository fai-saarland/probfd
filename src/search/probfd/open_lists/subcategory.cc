#include "probfd/open_lists/fifo_open_list_factory.h"
#include "probfd/open_lists/lifo_open_list_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace open_lists {

static class TaskOpenListFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskOpenListFactory> {
public:
    TaskOpenListFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskOpenListFactory")
    {
        document_synopsis("Factory for open lists");
    }
} _category_plugin;

class FifoOpenListFactoryFeature
    : public plugins::TypedFeature<TaskOpenListFactory, FifoOpenListFactory> {
public:
    FifoOpenListFactoryFeature()
        : TypedFeature("fifo_open_list_factory")
    {
    }

    std::shared_ptr<FifoOpenListFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<FifoOpenListFactory>();
    }
};

class LifoOpenListFactoryFeature
    : public plugins::TypedFeature<TaskOpenListFactory, LifoOpenListFactory> {
public:
    LifoOpenListFactoryFeature()
        : TypedFeature("lifo_open_list_factory")
    {
    }

    std::shared_ptr<LifoOpenListFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<LifoOpenListFactory>();
    }
};

static plugins::FeaturePlugin<FifoOpenListFactoryFeature> _plugin_fifo;
static plugins::FeaturePlugin<LifoOpenListFactoryFeature> _plugin_lifo;

} // namespace open_lists
} // namespace probfd