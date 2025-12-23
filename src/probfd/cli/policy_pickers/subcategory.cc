#include "probfd/cli/policy_pickers/subcategory.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"
#include "probfd/policy_pickers/operator_id_tiebreaker.h"
#include "probfd/policy_pickers/random_tiebreaker.h"
#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/policy_picker.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/operator_id.h"
#include "downward/state.h"
#include "downward/tuple_utils.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::policy_pickers;

using namespace probfd::cli;

using namespace language::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
template <
    template <typename, typename> typename S,
    bool Bisimulation,
    bool Fret>
using Wrapper = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        S<quotients::QuotientState<bisimulation::QuotientState, OperatorID>,
          quotients::QuotientAction<OperatorID>>,
        S<bisimulation::QuotientState, OperatorID>>,
    std::conditional_t<
        Fret,
        S<quotients::QuotientState<State, OperatorID>,
          quotients::QuotientAction<OperatorID>>,
        S<State, OperatorID>>>;

template <bool Bisimulation, bool Fret>
using PolicyPicker = Wrapper<algorithms::PolicyPicker, Bisimulation, Fret>;

template <bool Bisimulation, bool Fret>
InternalFunctionDefinitionBase&
add_arbitrary_policy_picker_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<Bisimulation, Fret>(
            "arbitrary_policy_tiebreaker"),
        &language::plugins::construct_shared<
            PolicyPicker<Bisimulation, Fret>,
            Wrapper<ArbitraryTiebreaker, Bisimulation, Fret>,
            bool>);

    f.make_optional_argument_with_default(0, "stable_policy", "true");

    return f;
}

InternalFunctionDefinitionBase&
add_operator_id_policy_picker_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "operator_id_policy_tiebreaker",
        &language::plugins::construct_shared<
            FDRPolicyPicker,
            OperatorIdTiebreaker,
            bool,
            bool>);

    f.make_optional_argument_with_default(0, "stable_policy", "true");
    f.make_optional_argument_with_default(1, "prefer_smaller", "true");

    return f;
}

template <bool Bisimulation, bool Fret>
InternalFunctionDefinitionBase&
add_random_tiebreaker_policy_picker_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<Bisimulation, Fret>("random_policy_tiebreaker"),
        &language::plugins::construct_shared<
            PolicyPicker<Bisimulation, Fret>,
            Wrapper<RandomTiebreaker, Bisimulation, Fret>,
            bool,
            int>);

    f.make_optional_argument_with_default(0, "stable_policy", "true");
    add_rng_options_to_feature(f, 1);

    return f;
}

template <bool Bisimulation, bool Fret>
InternalFunctionDefinitionBase&
add_value_gap_policy_picker_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        add_mdp_type_to_option<Bisimulation, Fret>(
            "value_gap_policy_tiebreaker"),
        &language::plugins::construct_shared<
            PolicyPicker<Bisimulation, Fret>,
            Wrapper<VDiffTiebreaker, Bisimulation, Fret>,
            bool,
            bool>);

    f.make_optional_argument_with_default(0, "stable_policy", "true");
    f.make_optional_argument_with_default(1, "prefer_large_gaps", "true");

    return f;
}

} // namespace

namespace probfd::cli::policy_pickers {

void add_policy_picker_category(Namespace& nspace)
{
    nspace.insert_shared_type_declarations<PolicyPicker>(
        []<bool Bisimulation, bool Fret> {
            return add_mdp_type_to_category<Bisimulation, Fret>("PolicyPicker");
        },
        []<bool, bool> { return "Tiebreaker for greedy actions."; });
}

void add_policy_picker_features(Namespace& nspace)
{
    add_arbitrary_policy_picker_to_namespace<false, false>(nspace);
    add_arbitrary_policy_picker_to_namespace<false, true>(nspace);
    add_arbitrary_policy_picker_to_namespace<true, false>(nspace);
    add_arbitrary_policy_picker_to_namespace<true, true>(nspace);

    add_operator_id_policy_picker_to_namespace(nspace);

    add_random_tiebreaker_policy_picker_to_namespace<false, false>(nspace);
    add_random_tiebreaker_policy_picker_to_namespace<false, true>(nspace);
    add_random_tiebreaker_policy_picker_to_namespace<true, false>(nspace);
    add_random_tiebreaker_policy_picker_to_namespace<true, true>(nspace);

    add_value_gap_policy_picker_to_namespace<false, false>(nspace);
    add_value_gap_policy_picker_to_namespace<false, true>(nspace);
    add_value_gap_policy_picker_to_namespace<true, false>(nspace);
    add_value_gap_policy_picker_to_namespace<true, true>(nspace);
}

} // namespace probfd::cli::policy_pickers
