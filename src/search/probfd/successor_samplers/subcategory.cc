#include "probfd/successor_samplers/arbitrary_sampler.h"
#include "probfd/successor_samplers/most_likely_sampler.h"
#include "probfd/successor_samplers/random_successor_sampler.h"
#include "probfd/successor_samplers/uniform_successor_sampler.h"
#include "probfd/successor_samplers/vbiased_successor_sampler.h"
#include "probfd/successor_samplers/vdiff_successor_sampler.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/plugins/naming_conventions.h"

#include "probfd/fdr_types.h"

#include "downward/task_proxy.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

namespace probfd {
namespace successor_samplers {

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

static SuccessorSamplerCategoryPlugin<false, false> _category_plugin;
static SuccessorSamplerCategoryPlugin<false, true> _category_plugin2;
static SuccessorSamplerCategoryPlugin<true, false> _category_plugin3;
static SuccessorSamplerCategoryPlugin<true, true> _category_plugin4;

static FeaturePlugin<ArbitrarySuccessorSamplerFeature<false, false>>
    _plugin_arbitary1;
static FeaturePlugin<ArbitrarySuccessorSamplerFeature<false, true>>
    _plugin_arbitary2;
static FeaturePlugin<ArbitrarySuccessorSamplerFeature<true, false>>
    _plugin_arbitary3;
static FeaturePlugin<ArbitrarySuccessorSamplerFeature<true, true>>
    _plugin_arbitary4;

static FeaturePlugin<MostLikelySuccessorSamplerFeature<false, false>>
    _plugin_most_likely1;
static FeaturePlugin<MostLikelySuccessorSamplerFeature<false, true>>
    _plugin_most_likely2;
static FeaturePlugin<MostLikelySuccessorSamplerFeature<true, false>>
    _plugin_most_likely3;
static FeaturePlugin<MostLikelySuccessorSamplerFeature<true, true>>
    _plugin_most_likely4;

static FeaturePlugin<RandomSuccessorSamplerFeature<false, false>>
    _plugin_random1;
static FeaturePlugin<RandomSuccessorSamplerFeature<false, true>>
    _plugin_random2;
static FeaturePlugin<RandomSuccessorSamplerFeature<true, false>>
    _plugin_random3;
static FeaturePlugin<RandomSuccessorSamplerFeature<true, true>> _plugin_random4;

static FeaturePlugin<UniformSuccessorSamplerFeature<false, false>>
    _plugin_uniform_random1;
static FeaturePlugin<UniformSuccessorSamplerFeature<false, true>>
    _plugin_uniform_random2;
static FeaturePlugin<UniformSuccessorSamplerFeature<true, false>>
    _plugin_uniform_random3;
static FeaturePlugin<UniformSuccessorSamplerFeature<true, true>>
    _plugin_uniform_random4;

static FeaturePlugin<VBiasedSuccessorSamplerFeature<false, false>>
    _plugin_vbiased1;
static FeaturePlugin<VBiasedSuccessorSamplerFeature<false, true>>
    _plugin_vbiased2;
static FeaturePlugin<VBiasedSuccessorSamplerFeature<true, false>>
    _plugin_vbiased3;
static FeaturePlugin<VBiasedSuccessorSamplerFeature<true, true>>
    _plugin_vbiased4;

static FeaturePlugin<VDiffSuccessorSamplerFeature<false, false>>
    _plugin_value_gap1;
static FeaturePlugin<VDiffSuccessorSamplerFeature<false, true>>
    _plugin_value_gap2;
static FeaturePlugin<VDiffSuccessorSamplerFeature<true, false>>
    _plugin_value_gap3;
static FeaturePlugin<VDiffSuccessorSamplerFeature<true, true>>
    _plugin_value_gap4;

} // namespace successor_samplers
} // namespace probfd