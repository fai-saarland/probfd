#include "downward/cli/tasks/cost_task_transformation_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/operator_cost_options.h"

#include "downward/tasks/cost_adapted_task.h"
#include "downward/tasks/root_task.h"

#include "downward/transformations/identity_transformation.h"

#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::tasks;

using namespace downward::cli::plugins;

using downward::cli::add_cost_type_options_to_feature;

namespace {
class CostAdaptedTaskTransformation : public TaskTransformation {
    OperatorCost cost_type;

public:
    explicit CostAdaptedTaskTransformation(OperatorCost cost_type)
        : cost_type(cost_type)
    {
    }

    TaskTransformationResult
    transform(const SharedAbstractTask& original_task) override
    {
        return {
            replace(
                original_task,
                std::make_shared<AdaptedOperatorIntCostFunction>(
                    get_operators(original_task),
                    get_shared_cost_function(original_task),
                    cost_type)),
            std::make_shared<IdentityStateMapping>(),
            std::make_shared<IdentityOperatorMapping>()};
    }
};

class CostAdaptedTaskTransformationFeature
    : public SharedTypedFeature<TaskTransformation, OperatorCost> {
public:
    CostAdaptedTaskTransformationFeature()
        : TypedFeature(
              "adapt_costs",
              &CostAdaptedTaskTransformationFeature::func)
    {
        document_title("Cost-adapted task");
        document_synopsis("A cost-adapting transformation of the root task.");

        add_cost_type_options_to_feature(*this, 0);
    }

    static shared_ptr<TaskTransformation>
    func(OperatorCost cost_type)
    {
        return make_shared_from_arg_tuples<CostAdaptedTaskTransformation>(
            cost_type);
    }
};
} // namespace

namespace downward::cli::tasks {

void add_cost_task_transformation_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<CostAdaptedTaskTransformationFeature>();
}

} // namespace downward::cli::tasks
