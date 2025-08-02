#include "probfd/cli/successor_samplers/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

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

using namespace downward;
using namespace downward::utils;

using namespace probfd;
using namespace probfd::cli;

using namespace probfd::successor_samplers;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
template <template <typename> typename S, bool Fret>
using Wrapper = std::conditional_t<
    Fret,
    S<quotients::QuotientAction<OperatorID>>,
    S<OperatorID>>;

template <bool Fret>
using SuccessorSampler = Wrapper<algorithms::SuccessorSampler, Fret>;

template <bool Fret>
class SuccessorSamplerCategoryPlugin
    : public TypedCategoryPlugin<SuccessorSampler<Fret>> {
public:
    SuccessorSamplerCategoryPlugin()
        : TypedCategoryPlugin<SuccessorSampler<Fret>>(
              add_mdp_type_to_category<false, Fret>("SuccessorSampler"))
    {
    }
};

template <bool Fret>
class ArbitrarySuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Fret>,
          Wrapper<ArbitrarySuccessorSampler, Fret>> {
public:
    ArbitrarySuccessorSamplerFeature()
        : ArbitrarySuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>(
                  "arbitrary_successor_sampler"))
    {
    }

    [[nodiscard]]
    std::shared_ptr<Wrapper<ArbitrarySuccessorSampler, Fret>>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<Wrapper<ArbitrarySuccessorSampler, Fret>>();
    }
};

template <bool Fret>
class MostLikelySuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Fret>,
          Wrapper<MostLikelySuccessorSampler, Fret>> {
public:
    MostLikelySuccessorSamplerFeature()
        : MostLikelySuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>(
                  "most_likely_successor_Sampler"))
    {
    }

    [[nodiscard]]
    std::shared_ptr<Wrapper<MostLikelySuccessorSampler, Fret>>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<Wrapper<MostLikelySuccessorSampler, Fret>>();
    }
};

template <bool Fret>
class UniformSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Fret>,
          Wrapper<UniformSuccessorSampler, Fret>> {
    using R = Wrapper<UniformSuccessorSampler, Fret>;

public:
    UniformSuccessorSamplerFeature()
        : UniformSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>(
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

template <bool Fret>
class RandomSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Fret>,
          Wrapper<RandomSuccessorSampler, Fret>> {
    using R = Wrapper<RandomSuccessorSampler, Fret>;

public:
    RandomSuccessorSamplerFeature()
        : RandomSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>("random_successor_sampler"))
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

template <bool Fret>
class VBiasedSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Fret>,
          Wrapper<VBiasedSuccessorSampler, Fret>> {
    using R = Wrapper<VBiasedSuccessorSampler, Fret>;

public:
    VBiasedSuccessorSamplerFeature()
        : VBiasedSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>("vbiased_successor_sampler"))
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

template <bool Fret>
class VDiffSuccessorSamplerFeature
    : public TypedFeature<
          SuccessorSampler<Fret>,
          Wrapper<VDiffSuccessorSampler, Fret>> {
    using R = Wrapper<VDiffSuccessorSampler, Fret>;

public:
    VDiffSuccessorSamplerFeature()
        : VDiffSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>(
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
}

namespace probfd::cli::successor_samplers {

void add_successor_sampler_features(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugins<SuccessorSamplerCategoryPlugin>();
    raw_registry.insert_feature_plugins<ArbitrarySuccessorSamplerFeature>();
    raw_registry.insert_feature_plugins<MostLikelySuccessorSamplerFeature>();
    raw_registry.insert_feature_plugins<RandomSuccessorSamplerFeature>();
    raw_registry.insert_feature_plugins<UniformSuccessorSamplerFeature>();
    raw_registry.insert_feature_plugins<VBiasedSuccessorSamplerFeature>();
    raw_registry.insert_feature_plugins<VDiffSuccessorSamplerFeature>();
}

} // namespace
