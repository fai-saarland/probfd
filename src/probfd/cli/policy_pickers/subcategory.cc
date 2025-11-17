#include "probfd/cli/policy_pickers/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

#include "downward/utils/rng_options.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::policy_pickers;

using namespace probfd::cli;

using namespace downward::cli::plugins;

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
class ArbitraryTieBreakerFeature
    : public SharedTypedFeature<PolicyPicker<Bisimulation, Fret>, bool> {
    using R = Wrapper<ArbitraryTiebreaker, Bisimulation, Fret>;

public:
    ArbitraryTieBreakerFeature()
        : ArbitraryTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "arbitrary_policy_tiebreaker"),
              &ArbitraryTieBreakerFeature::func)
    {
        this->make_optional_argument_with_default(0, "stable_policy", "true");
    }

    static std::shared_ptr<PolicyPicker<Bisimulation, Fret>>
    func(const Context&, bool stable_policy)
    {
        return std::make_shared<R>(stable_policy);
    }
};

class OperatorIDTieBreakerFeature
    : public SharedTypedFeature<FDRPolicyPicker, bool, bool> {
public:
    OperatorIDTieBreakerFeature()
        : TypedFeature(
              "operator_id_policy_tiebreaker",
              &OperatorIDTieBreakerFeature::func)
    {
        make_optional_argument_with_default(0, "stable_policy", "true");
        make_optional_argument_with_default(1, "prefer_smaller", "true");
    }

    static std::shared_ptr<FDRPolicyPicker>
    func(const Context&, bool stable_policy, bool prefer_smaller)
    {
        return make_shared_from_arg_tuples<OperatorIdTiebreaker>(
            stable_policy,
            prefer_smaller);
    }
};

template <bool Bisimulation, bool Fret>
class RandomTieBreakerFeature
    : public SharedTypedFeature<PolicyPicker<Bisimulation, Fret>, bool, int> {

    using R = Wrapper<RandomTiebreaker, Bisimulation, Fret>;

public:
    RandomTieBreakerFeature()
        : RandomTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "random_policy_tiebreaker"),
              &RandomTieBreakerFeature::func)
    {
        this->make_optional_argument_with_default(
            0,
            "stable_policy",
            "true");
        add_rng_options_to_feature(*this, 1);
    }

    static std::shared_ptr<PolicyPicker<Bisimulation, Fret>>
    func(const Context&, bool stable_policy, int random_seed)
    {
        return make_shared_from_arg_tuples<R>(stable_policy, random_seed);
    }
};

template <bool Bisimulation, bool Fret>
class ValueGapTieBreakerFeature
    : public SharedTypedFeature<PolicyPicker<Bisimulation, Fret>, bool, bool> {
    using R = Wrapper<VDiffTiebreaker, Bisimulation, Fret>;

public:
    ValueGapTieBreakerFeature()
        : ValueGapTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "value_gap_policy_tiebreaker"),
              &ValueGapTieBreakerFeature::func)
    {
        this->make_optional_argument_with_default(
            0,
            "stable_policy",
            "true");

        this->make_optional_argument_with_default(
            1,
            "prefer_large_gaps",
            "true");
    }

    static std::shared_ptr<PolicyPicker<Bisimulation, Fret>>
    func(const Context&, bool stable_policy, bool prefer_large_gaps)
    {
        return make_shared_from_arg_tuples<R>(stable_policy, prefer_large_gaps);
    }
};
} // namespace

namespace probfd::cli::policy_pickers {

void add_policy_picker_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugins<PolicyPicker>(
        []<bool Bisimulation, bool Fret> {
            return add_mdp_type_to_category<Bisimulation, Fret>("PolicyPicker");
        },
        []<bool, bool> { return "Tiebreaker for greedy actions."; });
}

void add_policy_picker_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<OperatorIDTieBreakerFeature>();
    n.insert_feature_plugins<ArbitraryTieBreakerFeature>();
    n.insert_feature_plugins<RandomTieBreakerFeature>();
    n.insert_feature_plugins<ValueGapTieBreakerFeature>();
}

} // namespace probfd::cli::policy_pickers
