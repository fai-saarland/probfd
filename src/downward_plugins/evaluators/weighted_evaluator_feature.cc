#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/evaluator_options.h"

#include "downward/evaluators/weighted_evaluator.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace weighted_evaluator;

using namespace downward_plugins::plugins;

using downward_plugins::add_evaluator_options_to_feature;
using downward_plugins::get_evaluator_arguments_from_options;

namespace {

class WeightedEvaluatorFeature
    : public TypedFeature<Evaluator, WeightedEvaluator> {
public:
    WeightedEvaluatorFeature()
        : TypedFeature("weight")
    {
        document_subcategory("evaluators_basic");
        document_title("Weighted evaluator");
        document_synopsis(
            "Multiplies the value of the evaluator with the given weight.");

        add_option<shared_ptr<Evaluator>>("eval", "evaluator");
        add_option<int>("weight", "weight");
        add_evaluator_options_to_feature(*this, "weight");
    }

    virtual shared_ptr<WeightedEvaluator>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<WeightedEvaluator>(
            opts.get<shared_ptr<Evaluator>>("eval"),
            opts.get<int>("weight"),
            get_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<WeightedEvaluatorFeature> _plugin;

} // namespace
