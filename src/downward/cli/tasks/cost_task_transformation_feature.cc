#include "downward/cli/plugins/plugin.h"

#include "downward/cli/operator_cost_options.h"

#include "downward/tasks/cost_adapted_task.h"
#include "downward/tasks/root_task.h"

#include "downward/transformations/identity_transformation.h"

#include "downward/task_transformation.h"

using namespace std;
using namespace tasks;

using namespace downward::cli::plugins;

using downward::cli::add_cost_type_options_to_feature;
using downward::cli::get_cost_type_arguments_from_options;

namespace {

class CostAdaptedTaskTransformation : public TaskTransformation {
    OperatorCost cost_type;

public:
    CostAdaptedTaskTransformation(OperatorCost cost_type)
        : cost_type(cost_type)
    {
    }

    TaskTransformationResult
    transform(const std::shared_ptr<AbstractTask>& original_task)
    {
        return {
            std::make_shared<CostAdaptedTask>(original_task, cost_type),
            std::make_shared<IdentityStateMapping>(),
            std::make_shared<IdentityOperatorMapping>()};
    }
};

class CostAdaptedTaskTransformationFeature
    : public TypedFeature<TaskTransformation, CostAdaptedTaskTransformation> {
public:
    CostAdaptedTaskTransformationFeature()
        : TypedFeature("adapt_costs")
    {
        document_title("Cost-adapted task");
        document_synopsis("A cost-adapting transformation of the root task.");

        add_cost_type_options_to_feature(*this);
    }

    virtual shared_ptr<CostAdaptedTaskTransformation>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<CostAdaptedTaskTransformation>(
            get_cost_type_arguments_from_options(opts));
    }
};

FeaturePlugin<CostAdaptedTaskTransformationFeature> _plugin;

} // namespace
