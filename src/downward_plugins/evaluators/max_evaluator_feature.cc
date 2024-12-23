#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/evaluators/combining_evaluator_options.h"

#include "downward/evaluators/max_evaluator.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace max_evaluator;

using namespace downward_plugins;
using namespace downward_plugins::plugins;

using downward_plugins::combining_evaluator ::
    add_combining_evaluator_options_to_feature;

using downward_plugins::combining_evaluator ::
    get_combining_evaluator_arguments_from_options;

namespace {

class MaxEvaluatorFeature : public TypedFeature<Evaluator, MaxEvaluator> {
public:
    MaxEvaluatorFeature()
        : TypedFeature("max")
    {
        document_subcategory("evaluators_basic");
        document_title("Max evaluator");
        document_synopsis("Calculates the maximum of the sub-evaluators.");
        add_combining_evaluator_options_to_feature(*this, "max");
    }

    virtual shared_ptr<MaxEvaluator>
    create_component(const Options& opts, const utils::Context& context)
        const override
    {
        verify_list_non_empty<shared_ptr<Evaluator>>(context, opts, "evals");
        return make_shared_from_arg_tuples<MaxEvaluator>(
            get_combining_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<MaxEvaluatorFeature> _plugin;

} // namespace
