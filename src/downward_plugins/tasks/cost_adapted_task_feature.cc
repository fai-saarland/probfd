#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/operator_cost_options.h"

#include "downward/tasks/cost_adapted_task.h"
#include "downward/tasks/root_task.h"

using namespace std;
using namespace tasks;

using namespace downward_plugins::plugins;

using downward_plugins::add_cost_type_options_to_feature;
using downward_plugins::get_cost_type_arguments_from_options;

namespace {

class CostAdaptedTaskFeature
    : public TypedFeature<AbstractTask, CostAdaptedTask> {
public:
    CostAdaptedTaskFeature()
        : TypedFeature("adapt_costs")
    {
        document_title("Cost-adapted task");
        document_synopsis("A cost-adapting transformation of the root task.");

        add_cost_type_options_to_feature(*this);
    }

    virtual shared_ptr<CostAdaptedTask>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<CostAdaptedTask>(
            g_root_task,
            get_cost_type_arguments_from_options(opts));
    }
};

FeaturePlugin<CostAdaptedTaskFeature> _plugin;

} // namespace
