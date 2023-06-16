#include "probfd/successor_samplers/arbitrary_selector_factory.h"
#include "probfd/successor_samplers/most_likely_selector_factory.h"
#include "probfd/successor_samplers/random_successor_sampler_factory.h"
#include "probfd/successor_samplers/uniform_successor_sampler_factory.h"
#include "probfd/successor_samplers/vbiased_successor_sampler_factory.h"
#include "probfd/successor_samplers/vdiff_successor_sampler_factory.h"

#include "plugins/plugin.h"

#include "utils/rng_options.h"

namespace probfd {
namespace successor_samplers {

static class TaskSuccessorSamplerFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskSuccessorSamplerFactory> {
public:
    TaskSuccessorSamplerFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskSuccessorSamplerFactory")
    {
    }
} _category_plugin_collection;

class ArbitrarySuccessorSelectorFactoryFeature
    : public plugins::TypedFeature<
          TaskSuccessorSamplerFactory,
          ArbitrarySuccessorSelectorFactory> {
public:
    ArbitrarySuccessorSelectorFactoryFeature()
        : TypedFeature("arbitrary_successor_selector_factory")
    {
    }

    std::shared_ptr<ArbitrarySuccessorSelectorFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<ArbitrarySuccessorSelectorFactory>();
    }
};

class MostLikelySuccessorSelectorFactoryFeature
    : public plugins::TypedFeature<
          TaskSuccessorSamplerFactory,
          MostLikelySuccessorSelectorFactory> {
public:
    MostLikelySuccessorSelectorFactoryFeature()
        : TypedFeature("most_likely_successor_selector_factory")
    {
    }

    std::shared_ptr<MostLikelySuccessorSelectorFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<MostLikelySuccessorSelectorFactory>();
    }
};

class RandomSuccessorSelectorFactoryFeature
    : public plugins::TypedFeature<
          TaskSuccessorSamplerFactory,
          RandomSuccessorSamplerFactory> {
public:
    RandomSuccessorSelectorFactoryFeature()
        : TypedFeature("random_successor_sampler_factory")
    {
        utils::add_rng_options(*this);
    }
};

class UniformSuccessorSelectorFactoryFeature
    : public plugins::TypedFeature<
          TaskSuccessorSamplerFactory,
          UniformSuccessorSamplerFactory> {
public:
    UniformSuccessorSelectorFactoryFeature()
        : TypedFeature("uniform_random_successor_sampler_factory")
    {
        utils::add_rng_options(*this);
    }
};

class VBiasedSuccessorSamplerFactoryFeature
    : public plugins::TypedFeature<
          TaskSuccessorSamplerFactory,
          VBiasedSuccessorSamplerFactory> {
public:
    VBiasedSuccessorSamplerFactoryFeature()
        : TypedFeature("vbiased_successor_sampler_factory")
    {
        utils::add_rng_options(*this);
    }
};

class VDiffSuccessorSamplerFactoryFeature
    : public plugins::TypedFeature<
          TaskSuccessorSamplerFactory,
          VDiffSuccessorSamplerFactory> {
public:
    VDiffSuccessorSamplerFactoryFeature()
        : TypedFeature("value_gap_successor_sampler_factory")
    {
        utils::add_rng_options(*this);
        add_option<bool>("prefer_large_gaps", "", "true");
    }
};

static plugins::FeaturePlugin<ArbitrarySuccessorSelectorFactoryFeature>
    _plugin_arbitary;

static plugins::FeaturePlugin<MostLikelySuccessorSelectorFactoryFeature>
    _plugin_most_likely;

static plugins::FeaturePlugin<RandomSuccessorSelectorFactoryFeature>
    _plugin_random;

static plugins::FeaturePlugin<UniformSuccessorSelectorFactoryFeature>
    _plugin_uniform_random;

static plugins::FeaturePlugin<VBiasedSuccessorSamplerFactoryFeature>
    _plugin_vbiased;

static plugins::FeaturePlugin<VDiffSuccessorSamplerFactoryFeature>
    _plugin_value_gap;

} // namespace successor_samplers
} // namespace probfd