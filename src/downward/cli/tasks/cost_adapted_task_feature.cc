#include "downward/cli/plugins/plugin.h"

#include "downward/cli/operator_cost_options.h"

#include "downward/tasks/cost_adapted_task.h"
#include "downward/tasks/root_task.h"

using namespace std;
using namespace downward::tasks;

using namespace downward::cli::plugins;

using downward::cli::add_cost_type_options_to_feature;
using downward::cli::get_cost_type_arguments_from_options;

namespace {

class CostAdaptedTaskFeature
    : public TypedFeature<downward::AbstractTask, CostAdaptedTask> {
public:
    CostAdaptedTaskFeature()
        : TypedFeature("adapt_costs")
    {
        document_title("Cost-adapted task");
        document_synopsis("A cost-adapting transformation of the root task.");

        add_cost_type_options_to_feature(*this);
    }

    virtual shared_ptr<CostAdaptedTask>
    create_component(const Options& opts, const downward::utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<CostAdaptedTask>(
            g_root_task,
            get_cost_type_arguments_from_options(opts));
    }
};

FeaturePlugin<CostAdaptedTaskFeature> _plugin;

} // namespace
