#include "downward/cli/plugins/plugin.h"

#include "downward/cli/evaluators/combining_evaluator_options.h"

#include "downward/evaluators/sum_evaluator.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace sum_evaluator;

using namespace downward::cli::plugins;

using downward::cli::combining_evaluator::
    add_combining_evaluator_options_to_feature;

using downward::cli::combining_evaluator::
    get_combining_evaluator_arguments_from_options;

namespace {

class SumEvaluatorFeature : public TypedFeature<Evaluator, SumEvaluator> {
public:
    SumEvaluatorFeature()
        : TypedFeature("sum")
    {
        document_subcategory("evaluators_basic");
        document_title("Sum evaluator");
        document_synopsis("Calculates the sum of the sub-evaluators.");

        add_combining_evaluator_options_to_feature(*this, "sum");
    }

    virtual shared_ptr<SumEvaluator>
    create_component(const Options& opts, const utils::Context& context)
        const override
    {
        verify_list_non_empty<shared_ptr<Evaluator>>(context, opts, "evals");
        return make_shared_from_arg_tuples<SumEvaluator>(
            get_combining_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<SumEvaluatorFeature> _plugin;

} // namespace
