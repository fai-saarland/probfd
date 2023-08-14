#include "probfd/open_lists/fifo_open_list_factory.h"
#include "probfd/open_lists/lifo_open_list_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace open_lists {

static class FDROpenListFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<FDROpenListFactory> {
public:
    FDROpenListFactoryCategoryPlugin()
        : TypedCategoryPlugin("FDROpenListFactory")
    {
        document_synopsis("Factory for open lists");
    }
} _category_plugin;

class FifoOpenListFactoryFeature
    : public plugins::TypedFeature<FDROpenListFactory, FifoOpenListFactory> {
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
    : public plugins::TypedFeature<FDROpenListFactory, LifoOpenListFactory> {
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