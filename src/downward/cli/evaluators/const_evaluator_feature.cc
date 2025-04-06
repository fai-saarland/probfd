#include "downward/cli/plugins/plugin.h"

#include "downward/cli/evaluator_options.h"

#include "downward/evaluators/const_evaluator.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::utils;
using namespace downward::const_evaluator;

using namespace downward::cli::plugins;

using downward::cli::add_evaluator_options_to_feature;
using downward::cli::get_evaluator_arguments_from_options;

namespace {

class ConstEvaluatorFeature
    : public TypedFeature<downward::Evaluator, ConstEvaluator> {
public:
    ConstEvaluatorFeature()
        : TypedFeature("const")
    {
        document_subcategory("evaluators_basic");
        document_title("Constant evaluator");
        document_synopsis("Returns a constant value.");

        add_option<int>(
            "value",
            "the constant value",
            "1",
            Bounds("0", "infinity"));
        add_evaluator_options_to_feature(*this, "const");
    }

    virtual shared_ptr<ConstEvaluator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<ConstEvaluator>(
            opts.get<int>("value"),
            get_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<ConstEvaluatorFeature> _plugin;

} // namespace
