#include "probfd/cli/policy_pickers/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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

#include "downward/utils/rng_options.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::policy_pickers;

using namespace probfd::cli;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

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
class PolicyPickerCategoryPlugin
    : public SharedTypedCategoryPlugin<PolicyPicker<Bisimulation, Fret>> {
public:
    PolicyPickerCategoryPlugin()
        : PolicyPickerCategoryPlugin::SharedTypedCategoryPlugin(
              add_mdp_type_to_category<Bisimulation, Fret>("PolicyPicker"))
    {
        this->document_synopsis("Tiebreaker for greedy actions.");
    }
};

template <bool Bisimulation, bool Fret>
class ArbitraryTieBreakerFeature
    : public SharedTypedFeature<PolicyPicker<Bisimulation, Fret>> {
    using R = Wrapper<ArbitraryTiebreaker, Bisimulation, Fret>;

public:
    ArbitraryTieBreakerFeature()
        : ArbitraryTieBreakerFeature::SharedTypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "arbitrary_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
    }

    std::shared_ptr<PolicyPicker<Bisimulation, Fret>>
    create_component(const Options& opts, const Context&) const override
    {
        return std::make_shared<R>(opts.get<bool>("stable_policy"));
    }
};

class OperatorIDTieBreakerFeature : public SharedTypedFeature<FDRPolicyPicker> {
public:
    OperatorIDTieBreakerFeature()
        : SharedTypedFeature("operator_id_policy_tiebreaker")
    {
        add_option<bool>("stable_policy", "", "true");
        add_option<bool>("prefer_smaller", "", "true");
    }

    std::shared_ptr<FDRPolicyPicker>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<OperatorIdTiebreaker>(
            opts.get<bool>("stable_policy"),
            opts.get<bool>("prefer_smaller"));
    }
};

template <bool Bisimulation, bool Fret>
class RandomTieBreakerFeature
    : public SharedTypedFeature<PolicyPicker<Bisimulation, Fret>> {

    using R = Wrapper<RandomTiebreaker, Bisimulation, Fret>;

public:
    RandomTieBreakerFeature()
        : RandomTieBreakerFeature::SharedTypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "random_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
        add_rng_options_to_feature(*this);
    }

    std::shared_ptr<PolicyPicker<Bisimulation, Fret>>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<R>(
            opts.get<bool>("stable_policy"),
            get_rng_arguments_from_options(opts));
    }
};

template <bool Bisimulation, bool Fret>
class ValueGapTieBreakerFeature
    : public SharedTypedFeature<PolicyPicker<Bisimulation, Fret>> {
    using R = Wrapper<VDiffTiebreaker, Bisimulation, Fret>;

public:
    ValueGapTieBreakerFeature()
        : ValueGapTieBreakerFeature::SharedTypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "value_gap_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
        this->template add_option<bool>("prefer_large_gaps", "", "true");
    }

    std::shared_ptr<PolicyPicker<Bisimulation, Fret>>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<R>(
            opts.get<bool>("stable_policy"),
            opts.get<bool>("prefer_large_gaps"));
    }
};
} // namespace

namespace probfd::cli::policy_pickers {

void add_policy_picker_features(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugins<PolicyPickerCategoryPlugin>();

    raw_registry.insert_feature_plugin<OperatorIDTieBreakerFeature>();
    raw_registry.insert_feature_plugins<ArbitraryTieBreakerFeature>();
    raw_registry.insert_feature_plugins<RandomTieBreakerFeature>();
    raw_registry.insert_feature_plugins<ValueGapTieBreakerFeature>();
}

} // namespace probfd::cli::policy_pickers
