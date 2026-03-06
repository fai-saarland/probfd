#include "downward/cli/tasks/cost_task_transformation_feature.h"

#include "language/ast/internal_function_definition.h"

#include "downward/cli/operator_cost_options.h"

#include "downward/tasks/cost_adapted_task.h"
#include "downward/tasks/root_task.h"

#include "downward/transformations/identity_transformation.h"

#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::tasks;

using namespace language::parser;

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

} // namespace

namespace downward::cli::tasks {

InternalFunctionDefinitionBase&
add_cost_task_transformation_features(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskTransformation,
        CostAdaptedTaskTransformation,
        OperatorCost>>(nspace, "adapt_costs");

    f.document_title("Cost-adapted task");
    f.document_synopsis("A cost-adapting transformation of the root task.");

    add_cost_type_options_to_feature(f, 0);

    return f;
}

} // namespace downward::cli::tasks
