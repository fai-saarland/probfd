#include "downward/tasks/cost_adapted_task.h"

#include "downward/operator_cost.h"

#include "downward/plugins/plugin.h"
#include "downward/task_utils/task_properties.h"
#include "downward/tasks/root_task.h"
#include "downward/utils/system.h"

#include <iostream>
#include <memory>

using namespace std;
using utils::ExitCode;

namespace tasks {
CostAdaptedTask::CostAdaptedTask(
    const shared_ptr<AbstractTask>& parent,
    OperatorCost cost_type)
    : DelegatingTask(parent)
    , cost_type(cost_type)
    , parent_is_unit_cost(task_properties::is_unit_cost(TaskProxy(*parent)))
{
}

int CostAdaptedTask::get_operator_cost(int index) const
{
    OperatorProxy op(*parent, index);
    return get_adjusted_action_cost(op, cost_type, parent_is_unit_cost);
}

class CostAdaptedTaskFeature
    : public plugins::TypedFeature<AbstractTask, CostAdaptedTask> {
public:
    CostAdaptedTaskFeature()
        : TypedFeature("adapt_costs")
    {
        document_title("Cost-adapted task");
        document_synopsis("A cost-adapting transformation of the root task.");

        add_cost_type_options_to_feature(*this);
    }

    virtual shared_ptr<CostAdaptedTask>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<CostAdaptedTask>(
            g_root_task,
            get_cost_type_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<CostAdaptedTaskFeature> _plugin;
} // namespace tasks
