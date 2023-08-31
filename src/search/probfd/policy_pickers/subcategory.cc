#include "probfd/policy_pickers/arbitrary_tiebreaker.h"
#include "probfd/policy_pickers/operator_id_tiebreaker.h"
#include "probfd/policy_pickers/random_tiebreaker.h"
#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "probfd/algorithms/fdr_types.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/plugins/multi_feature_plugin.h"
#include "probfd/plugins/naming_conventions.h"

#include "downward/task_proxy.h"

#include "downward/operator_id.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

namespace probfd {
namespace policy_pickers {
namespace {

using namespace plugins;

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
public:
    ArbitraryTieBreakerFeature()
        : ArbitraryTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "arbitrary_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
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
};

template <bool Bisimulation, bool Fret>
class RandomTieBreakerFeature
    : public TypedFeature<
          PolicyPicker<Bisimulation, Fret>,
          Wrapper<RandomTiebreaker, Bisimulation, Fret>> {
public:
    RandomTieBreakerFeature()
        : RandomTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "random_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
        utils::add_rng_options(*this);
    }
};

template <bool Bisimulation, bool Fret>
class ValueGapTieBreakerFeature
    : public TypedFeature<
          PolicyPicker<Bisimulation, Fret>,
          Wrapper<VDiffTiebreaker, Bisimulation, Fret>> {
public:
    ValueGapTieBreakerFeature()
        : ValueGapTieBreakerFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>(
                  "value_gap_policy_tiebreaker"))
    {
        this->template add_option<bool>("stable_policy", "", "true");
        this->template add_option<bool>("prefer_large_gaps", "", "true");
    }
};

static MultiCategoryPlugin<PolicyPickerCategoryPlugin> _category_plugin1;

static FeaturePlugin<OperatorIDTieBreakerFeature> _plugin_operator_id;

static MultiFeaturePlugin<ArbitraryTieBreakerFeature> _plugin_arbitary;
static MultiFeaturePlugin<RandomTieBreakerFeature> _plugin_random;
static MultiFeaturePlugin<ValueGapTieBreakerFeature> _plugin_value_gap;

} // namespace
} // namespace policy_pickers
} // namespace probfd