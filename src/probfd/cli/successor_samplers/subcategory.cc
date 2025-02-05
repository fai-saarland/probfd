#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/multi_feature_plugin.h"
#include "probfd/cli/naming_conventions.h"

#include "probfd/successor_samplers/arbitrary_sampler.h"
#include "probfd/successor_samplers/most_likely_sampler.h"
#include "probfd/successor_samplers/random_successor_sampler.h"
#include "probfd/successor_samplers/uniform_successor_sampler.h"
#include "probfd/successor_samplers/vbiased_successor_sampler.h"
#include "probfd/successor_samplers/vdiff_successor_sampler.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/operator_id.h"

#include <memory>

using namespace utils;

using namespace probfd;
using namespace probfd::cli;

using namespace probfd::successor_samplers;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

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

    [[nodiscard]]
    std::shared_ptr<Wrapper<ArbitrarySuccessorSampler, Bisimulation, Fret>>
    create_component(const Options&, const Context&) const override
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

    [[nodiscard]]
    std::shared_ptr<Wrapper<MostLikelySuccessorSampler, Bisimulation, Fret>>
    create_component(const Options&, const Context&) const override
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
    using R = Wrapper<UniformSuccessorSampler, Bisimulation, Fret>;

public:
    UniformSuccessorSamplerFeature()
        : UniformSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "uniform_random_successor_sampler"))
    {
        add_rng_options_to_feature(*this);
    }

    std::shared_ptr<R>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<R>(
            get_rng_arguments_from_options(opts));
    }
};

template <bool Bisimulation, bool Fret>
class RandomSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<RandomSuccessorSampler, Bisimulation, Fret>> {
    using R = Wrapper<RandomSuccessorSampler, Bisimulation, Fret>;

public:
    RandomSuccessorSamplerFeature()
        : RandomSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "random_successor_sampler"))
    {
        add_rng_options_to_feature(*this);
    }

    std::shared_ptr<R>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<R>(
            get_rng_arguments_from_options(opts));
    }
};

template <bool Bisimulation, bool Fret>
class VBiasedSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<VBiasedSuccessorSampler, Bisimulation, Fret>> {
    using R = Wrapper<VBiasedSuccessorSampler, Bisimulation, Fret>;

public:
    VBiasedSuccessorSamplerFeature()
        : VBiasedSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "vbiased_successor_sampler"))
    {
        add_rng_options_to_feature(*this);
    }

    std::shared_ptr<R>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<R>(
            get_rng_arguments_from_options(opts));
    }
};

template <bool Bisimulation, bool Fret>
class VDiffSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Bisimulation, Fret>,
          Wrapper<VDiffSuccessorSampler, Bisimulation, Fret>> {
    using R = Wrapper<VDiffSuccessorSampler, Bisimulation, Fret>;

public:
    VDiffSuccessorSamplerFeature()
        : VDiffSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "value_gap_successor_sampler"))
    {
        add_rng_options_to_feature(*this);
        this->template add_option<bool>("prefer_large_gaps", "", "true");
    }

    std::shared_ptr<R>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<R>(
            get_rng_arguments_from_options(opts),
            opts.get<bool>("prefer_large_gaps"));
    }
};

MultiCategoryPlugin<SuccessorSamplerCategoryPlugin> _category_plugin;

MultiFeaturePlugin<ArbitrarySuccessorSamplerFeature> _plugin_arbitary;
MultiFeaturePlugin<MostLikelySuccessorSamplerFeature> _plugin_most_likely;
MultiFeaturePlugin<RandomSuccessorSamplerFeature> _plugin_random;
MultiFeaturePlugin<UniformSuccessorSamplerFeature> _plugin_uniform_random;
MultiFeaturePlugin<VBiasedSuccessorSamplerFeature> _plugin_vbiased;
MultiFeaturePlugin<VDiffSuccessorSamplerFeature> _plugin_value_gap;

} // namespace
