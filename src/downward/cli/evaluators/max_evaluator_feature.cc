#include "downward/cli/plugins/plugin.h"

#include "downward/cli/evaluators/combining_evaluator_options.h"

#include "downward/evaluators/max_evaluator.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::max_evaluator;

using namespace downward::cli;
using namespace downward::cli::plugins;

using downward::cli::combining_evaluator ::
    add_combining_evaluator_options_to_feature;

using downward::cli::combining_evaluator ::
    get_combining_evaluator_arguments_from_options;

namespace {

class MaxEvaluatorFeature
    : public TypedFeature<downward::Evaluator, MaxEvaluator> {
public:
    MaxEvaluatorFeature()
        : TypedFeature("max")
    {
        document_subcategory("evaluators_basic");
        document_title("Max evaluator");
        document_synopsis("Calculates the maximum of the sub-evaluators.");
        add_combining_evaluator_options_to_feature(*this, "max");
    }

    virtual shared_ptr<MaxEvaluator> create_component(
        const Options& opts,
        const downward::utils::Context& context) const override
    {
        verify_list_non_empty<shared_ptr<downward::Evaluator>>(
            context,
            opts,
            "evals");
        return make_shared_from_arg_tuples<MaxEvaluator>(
            get_combining_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<MaxEvaluatorFeature> _plugin;

} // namespace
