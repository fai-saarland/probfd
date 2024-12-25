#include "downward/cli/plugins/plugin.h"

#include "downward/cli/evaluator_options.h"

#include "downward/evaluators/pref_evaluator.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace pref_evaluator;
using namespace utils;

using namespace downward::cli::plugins;

using downward::cli::add_evaluator_options_to_feature;
using downward::cli::get_evaluator_arguments_from_options;

namespace {

class PrefEvaluatorFeature : public TypedFeature<Evaluator, PrefEvaluator> {
public:
    PrefEvaluatorFeature()
        : TypedFeature("pref")
    {
        document_subcategory("evaluators_basic");
        document_title("Preference evaluator");
        document_synopsis("Returns 0 if preferred is true and 1 otherwise.");

        add_evaluator_options_to_feature(*this, "pref");
    }

    virtual shared_ptr<PrefEvaluator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PrefEvaluator>(
            get_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<PrefEvaluatorFeature> _plugin;

} // namespace
