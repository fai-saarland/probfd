#include "probfd/policy_pickers/arbitrary_tiebreaker.h"
#include "probfd/policy_pickers/operator_id_tiebreaker.h"
#include "probfd/policy_pickers/random_tiebreaker.h"
#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "operator_id.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_pickers {

static PluginTypePlugin<TaskPolicyPicker> _type_plugin("TaskPolicyPicker", "");

static std::shared_ptr<TaskPolicyPicker>
_parse_arbitrary(options::OptionParser& parser)
{
    if (parser.dry_run() || parser.help_mode()) return nullptr;
    parser.add_option<bool>("stable_policy", "", "true");
    Options opts = parser.parse();
    return std::make_shared<ArbitraryTiebreaker<State, OperatorID>>(opts);
}

static Plugin<TaskPolicyPicker>
    _plugin_arbitary("arbitrary_policy_tiebreaker", _parse_arbitrary);

static Plugin<TaskPolicyPicker> _plugin_operator_id(
    "operator_id_policy_tiebreaker",
    options::parse<TaskPolicyPicker, OperatorIdTiebreaker>);

static Plugin<TaskPolicyPicker> _plugin_random(
    "random_policy_tiebreaker",
    options::parse<TaskPolicyPicker, RandomTiebreaker>);

static Plugin<TaskPolicyPicker> _plugin_value_gap(
    "value_gap_policy_tiebreaker",
    options::parse<TaskPolicyPicker, VDiffTiebreaker>);

} // namespace policy_pickers
} // namespace probfd