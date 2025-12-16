#include "downward/cli/tasks/cost_task_transformation_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/operator_cost_options.h"

#include "downward/tasks/cost_adapted_task.h"
#include "downward/tasks/root_task.h"

#include "downward/transformations/identity_transformation.h"

#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::tasks;

using namespace language::plugins;

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

InternalFunctionDefinitionBase& add_cost_adapted_task_transformation_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "adapt_costs",
        &language::plugins::construct_shared<
            TaskTransformation,
            CostAdaptedTaskTransformation,
            OperatorCost>);
    f.document_title("Cost-adapted task");
    f.document_synopsis("A cost-adapting transformation of the root task.");

    add_cost_type_options_to_feature(f, 0);

    return f;
}

} // namespace

namespace downward::cli::tasks {

void add_cost_task_transformation_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_cost_adapted_task_transformation_to_namespace(n);
}

} // namespace downward::cli::tasks
