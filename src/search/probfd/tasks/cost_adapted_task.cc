#include "probfd/tasks/cost_adapted_task.h"

#include "probfd/task_utils/task_properties.h"

#include "operator_cost.h"
#include "option_parser.h"
#include "plugin.h"

#include "probfd/tasks/root_task.h"

#include "utils/system.h"

#include <iostream>
#include <memory>

using namespace std;
using utils::ExitCode;

namespace probfd {
namespace tasks {

CostAdaptedTask::CostAdaptedTask(
    const shared_ptr<ProbabilisticTask>& parent,
    OperatorCost cost_type)
    : DelegatingTask(parent)
    , cost_type(cost_type)
    , parent_is_unit_cost(
          task_properties::is_unit_cost(ProbabilisticTaskProxy(*parent)))
{
}

value_t CostAdaptedTask::get_operator_cost(int index) const
{
    ProbabilisticOperatorProxy op(*parent, index);
    return task_properties::get_adjusted_action_cost(
        op,
        cost_type,
        parent_is_unit_cost);
}

static shared_ptr<ProbabilisticTask> _parse(OptionParser& parser)
{
    parser.document_synopsis(
        "Cost-adapted task",
        "A cost-adapting transformation of the root task.");
    add_cost_type_option_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    } else {
        OperatorCost cost_type = opts.get<OperatorCost>("cost_type");
        return make_shared<CostAdaptedTask>(g_root_task, cost_type);
    }
}

static Plugin<ProbabilisticTask> _plugin("probabilistic_adapt_costs", _parse);

} // namespace tasks
} // namespace probfd