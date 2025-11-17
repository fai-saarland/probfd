#include "probfd/cli/successor_samplers/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

namespace {
template <template <typename> typename S, bool Fret>
using Wrapper = std::conditional_t<
    Fret,
    S<quotients::QuotientAction<OperatorID>>,
    S<OperatorID>>;

template <bool Fret>
using SuccessorSampler = Wrapper<algorithms::SuccessorSampler, Fret>;

template <bool Fret>
class ArbitrarySuccessorSamplerFeature
    : public SharedTypedFeature<SuccessorSampler<Fret>> {
public:
    ArbitrarySuccessorSamplerFeature()
        : ArbitrarySuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>(
                  "arbitrary_successor_sampler"),
              &ArbitrarySuccessorSamplerFeature::func)
    {
    }

    [[nodiscard]]
    static std::shared_ptr<SuccessorSampler<Fret>> func(const Context&)
    {
        return std::make_shared<Wrapper<ArbitrarySuccessorSampler, Fret>>();
    }
};

template <bool Fret>
class MostLikelySuccessorSamplerFeature
    : public SharedTypedFeature<SuccessorSampler<Fret>> {
public:
    MostLikelySuccessorSamplerFeature()
        : MostLikelySuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>(
                  "most_likely_successor_Sampler"),
              &MostLikelySuccessorSamplerFeature::func)
    {
    }

    [[nodiscard]]
    static std::shared_ptr<SuccessorSampler<Fret>> func(const Context&)
    {
        return std::make_shared<Wrapper<MostLikelySuccessorSampler, Fret>>();
    }
};

template <bool Fret>
class UniformSuccessorSamplerFeature
    : public SharedTypedFeature<SuccessorSampler<Fret>, int> {
public:
    UniformSuccessorSamplerFeature()
        : UniformSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>(
                  "uniform_random_successor_sampler"),
              &UniformSuccessorSamplerFeature::func)
    {
        add_rng_options_to_feature(*this, 0);
    }

    static std::shared_ptr<SuccessorSampler<Fret>>
    func(const Context&, int random_seed)
    {
        return make_shared_from_arg_tuples<
            Wrapper<UniformSuccessorSampler, Fret>>(random_seed);
    }
};

template <bool Fret>
class RandomSuccessorSamplerFeature
    : public SharedTypedFeature<SuccessorSampler<Fret>, int> {
public:
    RandomSuccessorSamplerFeature()
        : RandomSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>("random_successor_sampler"),
              &RandomSuccessorSamplerFeature::func)
    {
        add_rng_options_to_feature(*this, 0);
    }

    static std::shared_ptr<SuccessorSampler<Fret>>
    func(const Context&, int random_seed)
    {
        return make_shared_from_arg_tuples<
            Wrapper<RandomSuccessorSampler, Fret>>(random_seed);
    }
};

template <bool Fret>
class VBiasedSuccessorSamplerFeature
    : public SharedTypedFeature<SuccessorSampler<Fret>, int> {
public:
    VBiasedSuccessorSamplerFeature()
        : VBiasedSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>("vbiased_successor_sampler"),
              &VBiasedSuccessorSamplerFeature::func)
    {
        add_rng_options_to_feature(*this, 0);
    }

    static std::shared_ptr<SuccessorSampler<Fret>>
    func(const Context&, int random_seed)
    {
        return make_shared_from_arg_tuples<
            Wrapper<VBiasedSuccessorSampler, Fret>>(random_seed);
    }
};

template <bool Fret>
class VDiffSuccessorSamplerFeature
    : public SharedTypedFeature<SuccessorSampler<Fret>, int, bool> {
public:
    VDiffSuccessorSamplerFeature()
        : VDiffSuccessorSamplerFeature::TypedFeature(
              add_mdp_type_to_option<false, Fret>(
                  "value_gap_successor_sampler"),
              &VDiffSuccessorSamplerFeature::func)
    {
        const auto n = add_rng_options_to_feature(*this, 0);
        this->make_optional_argument_with_default(
            n,
            "prefer_large_gaps",
            "true");
    }

    static std::shared_ptr<SuccessorSampler<Fret>>
    func(const Context&, int random_seed, bool prefer_large_gaps)
    {
        return make_shared_from_arg_tuples<
            Wrapper<VDiffSuccessorSampler, Fret>>(
            random_seed,
            prefer_large_gaps);
    }
};
} // namespace

namespace probfd::cli::successor_samplers {

void add_successor_sampler_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugins<SuccessorSampler>(
        []<bool Fret> {
            return add_mdp_type_to_category<false, Fret>("SuccessorSampler");
        },
        []<bool> { return ""; });
}

void add_successor_sampler_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugins<ArbitrarySuccessorSamplerFeature>();
    n.insert_feature_plugins<MostLikelySuccessorSamplerFeature>();
    n.insert_feature_plugins<RandomSuccessorSamplerFeature>();
    n.insert_feature_plugins<UniformSuccessorSamplerFeature>();
    n.insert_feature_plugins<VBiasedSuccessorSamplerFeature>();
    n.insert_feature_plugins<VDiffSuccessorSamplerFeature>();
}

} // namespace probfd::cli::successor_samplers
