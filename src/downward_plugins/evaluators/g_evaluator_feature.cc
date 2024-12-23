#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/evaluator_options.h"

#include "downward/evaluators/g_evaluator.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace utils;
using namespace g_evaluator;

using namespace downward_plugins;
using namespace downward_plugins::plugins;

namespace {

class GEvaluatorFeature : public TypedFeature<Evaluator, GEvaluator> {
public:
    GEvaluatorFeature()
        : TypedFeature("g")
    {
        document_subcategory("evaluators_basic");
        document_title("g-value evaluator");
        document_synopsis(
            "Returns the g-value (path cost) of the search node.");
        add_evaluator_options_to_feature(*this, "g");
    }

    virtual shared_ptr<GEvaluator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<GEvaluator>(
            get_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<GEvaluatorFeature> _plugin;

} // namespace
