#include "probfd/policy_pickers/arbitrary_tiebreaker.h"
#include "probfd/policy_pickers/operator_id_tiebreaker.h"
#include "probfd/policy_pickers/random_tiebreaker.h"
#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "downward/operator_id.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

namespace probfd {
namespace policy_pickers {

static class TaskPolicyPickerCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskPolicyPicker> {
public:
    TaskPolicyPickerCategoryPlugin()
        : TypedCategoryPlugin("TaskPolicyPicker")
    {
        document_synopsis("Factory for policy action tiebreakers");
    }
} _category_plugin_collection;

class ArbitraryTieBreakerFeature
    : public plugins::TypedFeature<
          TaskPolicyPicker,
          ArbitraryTiebreaker<State, OperatorID>> {
public:
    ArbitraryTieBreakerFeature()
        : TypedFeature("arbitrary_policy_tiebreaker")
    {
        add_option<bool>("stable_policy", "", "true");
    }
};

class OperatorIDTieBreakerFeature
    : public plugins::TypedFeature<TaskPolicyPicker, OperatorIdTiebreaker> {
public:
    OperatorIDTieBreakerFeature()
        : TypedFeature("operator_id_policy_tiebreaker")
    {
        add_option<bool>("stable_policy", "", "true");
        add_option<bool>("prefer_smaller", "", "true");
    }
};

class RandomTieBreakerFeature
    : public plugins::TypedFeature<TaskPolicyPicker, RandomTiebreaker> {
public:
    RandomTieBreakerFeature()
        : TypedFeature("random_policy_tiebreaker")
    {
        add_option<bool>("stable_policy", "", "true");
        utils::add_rng_options(*this);
    }
};

class ValueGapTieBreakerFeature
    : public plugins::TypedFeature<TaskPolicyPicker, VDiffTiebreaker> {
public:
    ValueGapTieBreakerFeature()
        : TypedFeature("value_gap_policy_tiebreaker")
    {
        add_option<bool>("stable_policy", "", "true");
        add_option<bool>("prefer_large_gaps", "", "true");
    }
};

static plugins::FeaturePlugin<ArbitraryTieBreakerFeature> _plugin_arbitary;
static plugins::FeaturePlugin<OperatorIDTieBreakerFeature> _plugin_operator_id;
static plugins::FeaturePlugin<RandomTieBreakerFeature> _plugin_random;
static plugins::FeaturePlugin<ValueGapTieBreakerFeature> _plugin_value_gap;

} // namespace policy_pickers
} // namespace probfd