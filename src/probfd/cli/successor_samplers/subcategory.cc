#include "probfd/cli/successor_samplers/subcategory.h"

#include "language/ast/internal_function_definition.h"

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
#include "language/ast/internal_type_declaration.h"

#include <memory>

using namespace downward;
using namespace downward::utils;

using namespace probfd;
using namespace probfd::cli;

using namespace probfd::successor_samplers;

using namespace language::parser;

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
InternalFunctionDefinitionBase& add_arbitrary_successor_sampler_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        SuccessorSampler<Fret>,
        Wrapper<ArbitrarySuccessorSampler, Fret>>>(
        nspace,
        add_mdp_type_to_option<false, Fret>("arbitrary_successor_sampler"));

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase& add_most_likely_successor_sampler_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        SuccessorSampler<Fret>,
        Wrapper<MostLikelySuccessorSampler, Fret>>>(
        nspace,
        add_mdp_type_to_option<false, Fret>("most_likely_successor_Sampler"));

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase& add_uniform_successor_sampler_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        SuccessorSampler<Fret>,
        Wrapper<UniformSuccessorSampler, Fret>,
        std::shared_ptr<RandomNumberGenerator>>>(
        nspace,
        add_mdp_type_to_option<false, Fret>(
            "uniform_random_successor_sampler"));

    add_rng_options_to_feature(f, 0);

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase&
add_random_successor_sampler_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        SuccessorSampler<Fret>,
        Wrapper<RandomSuccessorSampler, Fret>,
        int>>(
        nspace,
        add_mdp_type_to_option<false, Fret>("random_successor_sampler"));

    add_rng_options_to_feature(f, 0);

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase& add_vbiased_successor_sampler_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        SuccessorSampler<Fret>,
        Wrapper<VBiasedSuccessorSampler, Fret>,
        int>>(
        nspace,
        add_mdp_type_to_option<false, Fret>("vbiased_successor_sampler"));

    add_rng_options_to_feature(f, 0);

    return f;
}

template <bool Fret>
InternalFunctionDefinitionBase&
add_vdiff_successor_sampler_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        SuccessorSampler<Fret>,
        Wrapper<VDiffSuccessorSampler, Fret>,
        int,
        bool>>(
        nspace,
        add_mdp_type_to_option<false, Fret>("value_gap_successor_sampler"));

    const auto n = add_rng_options_to_feature(f, 0);
    f.make_optional_argument_with_default(n, "prefer_large_gaps", "true");

    return f;
}

} // namespace

namespace probfd::cli::successor_samplers {

void add_successor_sampler_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declarations<SuccessorSampler>(
        nspace,
        []<bool Fret> {
            return add_mdp_type_to_category<false, Fret>("SuccessorSampler");
        },
        []<bool> { return ""; });
}

void add_successor_sampler_features(NamespaceLevelDeclarationList& nspace)
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
