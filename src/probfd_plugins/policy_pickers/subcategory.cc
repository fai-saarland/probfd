#include "probfd_plugins/multi_feature_plugin.h"
#include "probfd_plugins/naming_conventions.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"
#include "probfd/policy_pickers/operator_id_tiebreaker.h"
#include "probfd/policy_pickers/random_tiebreaker.h"
#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/policy_picker.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/task_proxy.h"

#include "downward/operator_id.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

using namespace plugins;

using namespace probfd;
using namespace probfd::policy_pickers;

using namespace probfd_plugins;

namespace {

template <
    template <typename, typename>
    typename S,
    bool Bisimulation,
    bool Fret>
using Wrapper = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        S<quotients::QuotientState<
              bisimulation::QuotientState,
              bisimulation::QuotientAction>,
          quotients::QuotientAction<bisimulation::QuotientAction>>,
        S<bisimulation::QuotientState, bisimulation::QuotientAction>>,
    std::conditional_t<
        Fret,
        S<quotients::QuotientState<State, OperatorID>,
          quotients::QuotientAction<OperatorID>>,
        S<State, OperatorID>>>;

template <bool Bisimulation, bool Fret>
using PolicyPicker = Wrapper<algorithms::PolicyPicker, Bisimulation, Fret>;

template <bool Bisimulation, bool Fret>
class PolicyPickerCategoryPlugin
    : public TypedCategoryPlugin<PolicyPicker<Bisimulation, Fret>> {
public:
    PolicyPickerCategoryPlugin()
        : PolicyPickerCategoryPlugin::TypedCategoryPlugin(
              add_mdp_type_to_category<Bisimulation, Fret>("PolicyPicker"))
    {
        this->document_synopsis("Tiebreaker for greedy actions.");
    }
};

template <bool Bisimulation, bool Fret>
class ArbitraryTieBreakerFeature
    : public TypedFeature<
          PolicyPicker<Bisimulation, Fret>,
          Wrapper<ArbitraryTiebreaker, Bisimulation, Fret>> {
    using R = Wrapper<ArbitraryTiebreaker, Bisimulation, Fret>;

public:
    ArbitraryTieBreakerFeature()
        : ArbitraryTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "arbitrary_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
    }

    std::shared_ptr<R>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return std::make_shared<R>(opts.get<bool>("stable_policy"));
    }
};

class OperatorIDTieBreakerFeature
    : public TypedFeature<FDRPolicyPicker, OperatorIdTiebreaker> {
public:
    OperatorIDTieBreakerFeature()
        : TypedFeature("operator_id_policy_tiebreaker")
    {
        add_option<bool>("stable_policy", "", "true");
        add_option<bool>("prefer_smaller", "", "true");
    }

    std::shared_ptr<OperatorIdTiebreaker>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<OperatorIdTiebreaker>(
            opts.get<bool>("stable_policy"),
            opts.get<bool>("prefer_smaller"));
    }
};

template <bool Bisimulation, bool Fret>
class RandomTieBreakerFeature
    : public TypedFeature<
          PolicyPicker<Bisimulation, Fret>,
          Wrapper<RandomTiebreaker, Bisimulation, Fret>> {

    using R = Wrapper<RandomTiebreaker, Bisimulation, Fret>;

public:
    RandomTieBreakerFeature()
        : RandomTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "random_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
        utils::add_rng_options_to_feature(*this);
    }

    std::shared_ptr<R>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<R>(
            opts.get<bool>("stable_policy"),
            utils::get_rng_arguments_from_options(opts));
    }
};

template <bool Bisimulation, bool Fret>
class ValueGapTieBreakerFeature
    : public TypedFeature<
          PolicyPicker<Bisimulation, Fret>,
          Wrapper<VDiffTiebreaker, Bisimulation, Fret>> {
    using R = Wrapper<VDiffTiebreaker, Bisimulation, Fret>;

public:
    ValueGapTieBreakerFeature()
        : ValueGapTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "value_gap_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
        this->template add_option<bool>("prefer_large_gaps", "", "true");
    }

    std::shared_ptr<R>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<R>(
            opts.get<bool>("stable_policy"),
            opts.get<bool>("prefer_large_gaps"));
    }
};

MultiCategoryPlugin<PolicyPickerCategoryPlugin> _category_plugin1;

FeaturePlugin<OperatorIDTieBreakerFeature> _plugin_operator_id;

MultiFeaturePlugin<ArbitraryTieBreakerFeature> _plugin_arbitary;
MultiFeaturePlugin<RandomTieBreakerFeature> _plugin_random;
MultiFeaturePlugin<ValueGapTieBreakerFeature> _plugin_value_gap;

} // namespace probfd::policy_pickers
