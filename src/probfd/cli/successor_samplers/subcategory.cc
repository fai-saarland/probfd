#include "probfd/cli/successor_samplers/subcategory.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/successor_samplers/arbitrary_sampler.h"
#include "probfd/successor_samplers/most_likely_sampler.h"
#include "probfd/successor_samplers/random_successor_sampler.h"
#include "probfd/successor_samplers/uniform_successor_sampler.h"
#include "probfd/successor_samplers/vbiased_successor_sampler.h"
#include "probfd/successor_samplers/vdiff_successor_sampler.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/operator_id.h"

#include <memory>

using namespace downward;
using namespace downward::utils;

using namespace probfd;
using namespace probfd::cli;

using namespace probfd::successor_samplers;

using namespace language::plugins;

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
InternalFunctionDefinitionBase&
add_arbitrary_successor_sampler_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<false, Fret>("arbitrary_successor_sampler"),
        &language::plugins::construct_shared<
            SuccessorSampler<Fret>,
            Wrapper<ArbitrarySuccessorSampler, Fret>>);

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase&
add_most_likely_successor_sampler_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<false, Fret>("most_likely_successor_Sampler"),
        &language::plugins::construct_shared<
            SuccessorSampler<Fret>,
            Wrapper<MostLikelySuccessorSampler, Fret>>);

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase&
add_uniform_successor_sampler_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<false, Fret>("uniform_random_successor_sampler"),
        &language::plugins::construct_shared<
            SuccessorSampler<Fret>,
            Wrapper<UniformSuccessorSampler, Fret>,
            std::shared_ptr<RandomNumberGenerator>>);

    add_rng_options_to_feature(f, 0);

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase&
add_random_successor_sampler_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<false, Fret>("random_successor_sampler"),
        &language::plugins::construct_shared<
            SuccessorSampler<Fret>,
            Wrapper<RandomSuccessorSampler, Fret>,
            int>);

    add_rng_options_to_feature(f, 0);

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase&
add_vbiased_successor_sampler_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<false, Fret>("vbiased_successor_sampler"),
        &language::plugins::construct_shared<
            SuccessorSampler<Fret>,
            Wrapper<VBiasedSuccessorSampler, Fret>,
            int>);

    add_rng_options_to_feature(f, 0);

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase&
add_vdiff_successor_sampler_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<false, Fret>("value_gap_successor_sampler"),
        &language::plugins::construct_shared<
            SuccessorSampler<Fret>,
            Wrapper<VDiffSuccessorSampler, Fret>,
            int,
            bool>);

    const auto n = add_rng_options_to_feature(f, 0);
    f.make_optional_argument_with_default(n, "prefer_large_gaps", "true");

    return f;
}

} // namespace

namespace probfd::cli::successor_samplers {

void add_successor_sampler_category(Namespace& nspace)
{
    nspace.insert_shared_type_declarations<SuccessorSampler>(
        []<bool Fret> {
            return add_mdp_type_to_category<false, Fret>("SuccessorSampler");
        },
        []<bool> { return ""; });
}

void add_successor_sampler_features(Namespace& nspace)
{
    add_arbitrary_successor_sampler_to_namespace<true>(nspace);
    add_arbitrary_successor_sampler_to_namespace<false>(nspace);
    add_most_likely_successor_sampler_to_namespace<true>(nspace);
    add_most_likely_successor_sampler_to_namespace<false>(nspace);
    add_random_successor_sampler_to_namespace<true>(nspace);
    add_random_successor_sampler_to_namespace<false>(nspace);
    add_uniform_successor_sampler_to_namespace<true>(nspace);
    add_uniform_successor_sampler_to_namespace<false>(nspace);
    add_vbiased_successor_sampler_to_namespace<true>(nspace);
    add_vbiased_successor_sampler_to_namespace<false>(nspace);
    add_vdiff_successor_sampler_to_namespace<true>(nspace);
    add_vdiff_successor_sampler_to_namespace<false>(nspace);
}

} // namespace probfd::cli::successor_samplers
