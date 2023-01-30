#include "probfd/policy_picker/arbitrary_tiebreaker_factory.h"
#include "probfd/policy_picker/operator_id_tiebreaker_factory.h"
#include "probfd/policy_picker/random_tiebreaker_factory.h"
#include "probfd/policy_picker/vdiff_tiebreaker_factory.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_tiebreaking {

static PluginTypePlugin<TaskPolicyPickerFactory>
    _type_plugin("TaskPolicyPickerFactory", "");

static std::shared_ptr<TaskPolicyPickerFactory>
_parse_arbitrary(options::OptionParser& parser)
{
    if (parser.dry_run() || parser.help_mode()) return nullptr;
    return std::make_shared<ArbitraryTiebreakerFactory>();
}

static Plugin<TaskPolicyPickerFactory>
    _plugin_arbitary("arbitrary_policy_tiebreaker_factory", _parse_arbitrary);

static Plugin<TaskPolicyPickerFactory> _plugin_operator_id(
    "operator_id_policy_tiebreaker_factory",
    options::parse<TaskPolicyPickerFactory, OperatorIdTiebreakerFactory>);

static Plugin<TaskPolicyPickerFactory> _plugin_random(
    "random_policy_tiebreaker_factory",
    options::parse<TaskPolicyPickerFactory, RandomTiebreakerFactory>);

static Plugin<TaskPolicyPickerFactory> _plugin_value_gap(
    "value_gap_policy_tiebreaker_factory",
    options::parse<TaskPolicyPickerFactory, VDiffTiebreakerFactory>);

} // namespace policy_tiebreaking
} // namespace probfd