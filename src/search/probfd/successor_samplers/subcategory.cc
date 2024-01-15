#include "probfd/successor_samplers/arbitrary_sampler.h"
#include "probfd/successor_samplers/most_likely_sampler.h"
#include "probfd/successor_samplers/random_successor_sampler.h"
#include "probfd/successor_samplers/uniform_successor_sampler.h"
#include "probfd/successor_samplers/vbiased_successor_sampler.h"
#include "probfd/successor_samplers/vdiff_successor_sampler.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/plugins/multi_feature_plugin.h"
#include "probfd/plugins/naming_conventions.h"

#include "probfd/fdr_types.h"

#include "downward/task_proxy.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

#include <memory>
#include <type_traits>

namespace probfd {
namespace successor_samplers {
namespace {

using namespace plugins;

template <template <typename> typename S, bool Bisimulation, bool Fret>
using Wrapper = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        S<quotients::QuotientAction<bisimulation::QuotientAction>>,
        S<bisimulation::QuotientAction>>,
    std::conditional_t<
        Fret,
        S<quotients::QuotientAction<OperatorID>>,
        S<OperatorID>>>;

template <bool Bisimulation, bool Fret>
using SuccessorSampler =
    Wrapper<algorithms::SuccessorSampler, Bisimulation, Fret>;

template <bool Bisimulation, bool Fret>
class SuccessorSamplerCategoryPlugin
    : public TypedCategoryPlugin<SuccessorSampler<Bisimulation, Fret>> {
public:
    SuccessorSamplerCategoryPlugin()
        : TypedCategoryPlugin<SuccessorSampler<Bisimulation, Fret>>(
              add_mdp_type_to_category<Bisimulation, Fret>("SuccessorSampler"))
    {
    }
};

template <bool Bisimulation, bool Fret>
class ArbitrarySuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<ArbitrarySuccessorSampler, Bisimulation, Fret>> {
public:
    ArbitrarySuccessorSamplerFeature()
        : ArbitrarySuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "arbitrary_successor_sampler"))
    {
    }

    std::shared_ptr<Wrapper<ArbitrarySuccessorSampler, Bisimulation, Fret>>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<
            Wrapper<ArbitrarySuccessorSampler, Bisimulation, Fret>>();
    }
};

template <bool Bisimulation, bool Fret>
class MostLikelySuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<MostLikelySuccessorSampler, Bisimulation, Fret>> {
public:
    MostLikelySuccessorSamplerFeature()
        : MostLikelySuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "most_likely_successor_Sampler"))
    {
    }

    std::shared_ptr<Wrapper<MostLikelySuccessorSampler, Bisimulation, Fret>>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<
            Wrapper<MostLikelySuccessorSampler, Bisimulation, Fret>>();
    }
};

template <bool Bisimulation, bool Fret>
class UniformSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<UniformSuccessorSampler, Bisimulation, Fret>> {
public:
    UniformSuccessorSamplerFeature()
        : UniformSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "uniform_random_successor_sampler"))
    {
        utils::add_rng_options(*this);
    }
};

template <bool Bisimulation, bool Fret>
class RandomSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<RandomSuccessorSampler, Bisimulation, Fret>> {
public:
    RandomSuccessorSamplerFeature()
        : RandomSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "random_successor_sampler"))
    {
        utils::add_rng_options(*this);
    }
};

template <bool Bisimulation, bool Fret>
class VBiasedSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<VBiasedSuccessorSampler, Bisimulation, Fret>> {
public:
    VBiasedSuccessorSamplerFeature()
        : VBiasedSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "vbiased_successor_sampler"))
    {
        utils::add_rng_options(*this);
    }
};

template <bool Bisimulation, bool Fret>
class VDiffSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<VDiffSuccessorSampler, Bisimulation, Fret>> {
public:
    VDiffSuccessorSamplerFeature()
        : VDiffSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "value_gap_successor_sampler"))
    {
        utils::add_rng_options(*this);
        this->template add_option<bool>("prefer_large_gaps", "", "true");
    }
};

static MultiCategoryPlugin<SuccessorSamplerCategoryPlugin> _category_plugin;

static MultiFeaturePlugin<ArbitrarySuccessorSamplerFeature> _plugin_arbitary;
static MultiFeaturePlugin<MostLikelySuccessorSamplerFeature>
    _plugin_most_likely;
static MultiFeaturePlugin<RandomSuccessorSamplerFeature> _plugin_random;
static MultiFeaturePlugin<UniformSuccessorSamplerFeature>
    _plugin_uniform_random;
static MultiFeaturePlugin<VBiasedSuccessorSamplerFeature> _plugin_vbiased;
static MultiFeaturePlugin<VDiffSuccessorSamplerFeature> _plugin_value_gap;

} // namespace
} // namespace successor_samplers
} // namespace probfd